#pragma once
#include "mozi/compile/attributes_cpp.hpp"
#include "mozi/core/actor/mail_out.hpp"
#include "mozi/core/coro/future.hpp"
#include "mozi/core/ring/event_factory.hpp"
#include "mozi/core/ring/event_translator.hpp"
#include "spdlog/spdlog.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>

namespace mozi::actor
{
// TODO 这里MMU 寻址优化
// 增加 void *data 的内存对齐
struct mo_mail_s
{
  public:
    friend class mo_mail_translator_c;
    inline bool read() noexcept
    {
        if (m_behavior_type == 0)
        {
#ifndef NDEBUG
            spdlog::error("mo_mail_s::read() behavior not set");
#endif
            return false;
        }
        if (m_behavior_type == 1)
        {
            block_read();
            return true;
        }
        else if (m_behavior_type == 2)
        {
            async_read();
            return true;
        }
        std::unreachable();
    }

    ~mo_mail_s()
    {
        spdlog::info("mo_mail_s::~mo_mail_s()");
        delete[] m_serial_in;
        free(m_in);
        if (m_out != nullptr)
        {
            m_out->destroy();
        }
        delete m_out;
    }
    mo_mail_s &operator=(const mo_mail_s &) = delete;
    mo_mail_s &operator=(mo_mail_s &&) = delete;
    mo_mail_s(const mo_mail_s &) = delete;
    mo_mail_s(mo_mail_s &&) = delete;
    mo_mail_s() = default;

  private:
    inline void init_mail(uint8_t *serial_mail_in,          //
                          void *mail_in,                    //
                          mo_mail_out_s *mail_out) noexcept //
    {
        delete[] m_serial_in;
        free(m_in);
        if (m_out != nullptr)
        {
            m_out->destroy();
        }
        delete m_out;

        m_serial_in = serial_mail_in;
        m_in = mail_in;
        m_out = mail_out;
        m_behavior_type = 0;
    }
    inline bool is_waiting() noexcept
    {
        return m_out != nullptr;
    }
    inline void set_behavior(void (*behavior)(uint8_t *, void *, mo_mail_out_s *)) noexcept
    {
        m_behavior_type = 1;
        m_block_behavior = behavior;
    }
    inline void set_behavior(coro::mo_future_s (*behavior)(uint8_t *, void *, mo_mail_out_s *)) noexcept
    {
        m_behavior_type = 2;
        m_async_behavior = behavior;
    }
    inline void block_read() noexcept
    {
        m_block_behavior(m_serial_in, m_in, m_out);
    }
    inline coro::mo_future_s async_read() noexcept
    {
        return m_async_behavior(m_serial_in, m_in, m_out);
    }

    // TODO: 这里对于behvavior 设置为nullptr的运行 actor自带的 行为匹配
    // https://actor-framework.readthedocs.io/en/stable/MessageHandlers.html
    // 需要一套完整的序列化定义 但是这个可以在二期的时候做
    void (*m_block_behavior)(uint8_t *, void *, mo_mail_out_s *) = nullptr;
    coro::mo_future_s (*m_async_behavior)(uint8_t *, void *, mo_mail_out_s *) = nullptr;

    uint8_t m_behavior_type = 0; // 0 not set 1 block 2 async
    uint8_t *m_serial_in = nullptr;
    void *m_in = nullptr;
    mo_mail_out_s *m_out = nullptr;
};

class mo_mail_factory_c : public mozi::ring::mo_event_factory_c<mo_mail_factory_c, mo_mail_s>
{
  public:
    mo_mail_factory_c() = default;
    [[MO_NODISCARD]] static std::unique_ptr<mo_mail_s> create_instance() noexcept
    {
        // spdlog::info("mo_mail_factory_c::create_instance");
        return std::make_unique<mozi::actor::mo_mail_s>();
    }
};

class mo_mail_translator_c : public mozi::ring::mo_event_translator_c<mo_mail_translator_c, mo_mail_s>
{
  public:
    void operator()(mo_mail_s *event, [[MO_UNUSED]] size_t sequence) noexcept
    {
#ifndef NDEBUG
        spdlog::debug("mo_mail_translator_c::operator()");
#endif
        event->init_mail(m_serial_in, m_in, m_out);
        m_behavior_type == 1 ? event->set_behavior(m_block_behavior) : event->set_behavior(m_async_behavior);
    }
    explicit mo_mail_translator_c(uint8_t *serial_mail_in, //
                                  void *mail_in,           //
                                  mo_mail_out_s *mail_out, //
                                  void (*block_behavior)(uint8_t *, void *, mo_mail_out_s *)) noexcept
        : m_serial_in(serial_mail_in),      //
          m_in(mail_in),                    //
          m_out(mail_out),                  //
          m_block_behavior(block_behavior), //
          m_behavior_type(1)                //
    {
        // TODO 检查不要是空指针
    }
    explicit mo_mail_translator_c(uint8_t *serial_mail_in, //
                                  void *mail_in,           //
                                  mo_mail_out_s *mail_out, //
                                  coro::mo_future_s (*async_behavior)(uint8_t *, void *, mo_mail_out_s *)) noexcept
        : m_serial_in(serial_mail_in),      //
          m_in(mail_in),                    //
          m_out(mail_out),                  //
          m_async_behavior(async_behavior), //
          m_behavior_type(2)                //
    {
        // TODO 检查不要是空指针
    }

    mo_mail_translator_c &operator=(const mo_mail_translator_c &) = delete;
    mo_mail_translator_c &operator=(mo_mail_translator_c &&) = delete;
    mo_mail_translator_c(const mo_mail_translator_c &) = delete;
    mo_mail_translator_c(mo_mail_translator_c &&) = delete;
    ~mo_mail_translator_c() = default;

  private:
    uint8_t *m_serial_in = nullptr;
    void *m_in = nullptr;
    mo_mail_out_s *m_out = nullptr;
    void (*m_block_behavior)(uint8_t *, void *, mo_mail_out_s *) = nullptr;
    coro::mo_future_s (*m_async_behavior)(uint8_t *, void *, mo_mail_out_s *) = nullptr;
    uint8_t m_behavior_type = 0; // 0 not set 1 block 2 async
};

struct mo_mail_handle_s
{
  public:
    bool on_event([[MO_UNUSED]] mo_mail_s *event, [[MO_UNUSED]] size_t sequence,
                  [[MO_UNUSED]] bool end_of_batch) noexcept
    {
        // TODO: 完善 增加日志 增加执行结果处理 增加异常处理
        // TODO 这里应该为协程支持
        return event->read();
    }
};

} // namespace mozi::actor