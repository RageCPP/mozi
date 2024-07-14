#pragma once
#include "mozi/compile/attributes_cpp.hpp"
#include "mozi/core/ring/event_factory.hpp"
#include "mozi/core/ring/event_translator.hpp"
#include "spdlog/spdlog.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>

namespace mozi::mail
{
// TODO 这里MMU 寻址优化
// 增加 void *data 的内存对齐
struct mo_mail_s
{
  private:
    struct mo__async_run_s
    {
    };

  public:
    friend class mo_mail_translator_c;
    struct mo_mail_out_s
    {
      public:
        void set_destroy(void (*destroy)(void *) noexcept)
        {
            m_destroy = destroy;
        }
        void set_check(void (*check)(void *, int64_t, uint16_t) noexcept)
        {
            m_check = check;
        }
        void set_data(void *data, uint16_t wait_senconds)
        {
            // TODO: m_start 时间戳
            m_wait_senconds = wait_senconds;
            m_start = 0;
            m_data = data;
        }
        void destroy() noexcept
        {
            if (m_destroy != nullptr)
            {
                m_destroy(m_data);
            }
        }
        void check(void *data, int64_t start, uint16_t wait_senconds) noexcept
        {
            // unsafe check
            // if (m_check != nullptr)
            // {
            m_check(data, start, wait_senconds);
            // }
        }
        ~mo_mail_out_s()
        {
            if (m_destroy != nullptr)
            {
                m_destroy(m_data);
            }
        }
        mo_mail_out_s &operator=(const mo_mail_out_s &) = delete;
        mo_mail_out_s &operator=(mo_mail_out_s &&) = delete;
        mo_mail_out_s(const mo_mail_out_s &) = delete;
        mo_mail_out_s(mo_mail_out_s &&) = delete;
        mo_mail_out_s() = default;

      private:
        int64_t m_start;
        uint16_t m_wait_senconds;
        void *m_data;
        void (*m_destroy)(void *) noexcept;
        void (*m_check)(void *, int64_t, uint16_t) noexcept;
    };
    void operator()() noexcept
    {
        m_behavior(m_serial_in, m_in);
    }
    // void operator()([[MO_UNUSED]] mo__async_run_s _) noexcept
    // {
    //     auto future = m_future_behavior(m_serial_in, m_in);
    //     future.handle().destroy();
    //     spdlog::info("mo__async_run end");
    // }
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
    struct mo__empty_s
    {
    };
    void store_mail_in(uint8_t *serial_mail_in, void *mail_in)
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
    }
    void store_mail_out(mo_mail_out_s *mail_out)
    {
        m_out = mail_out;
    }
    inline bool is_ready() noexcept
    {
        return m_out != nullptr;
    }
    inline void set_behavior(void (*behavior)(uint8_t *, void *)) noexcept
    {
        m_behavior = behavior;
    }
    // inline void set_behavior(coro::mo_future_s<std::unique_ptr<mo__empty_s>> (*behavior)(uint8_t *, void *)) noexcept
    // {
    //     m_future_behavior = behavior;
    // }

    // behavior must be set before calling the operator
    void (*m_behavior)(uint8_t *, void *) = nullptr;
    // coro::mo_future_s<std::unique_ptr<mo__empty_s>> (*m_future_behavior)(uint8_t *, void *) = nullptr;
    // behavior must be set before calling the operator

    // void (*m_store_mail_out)(void *) = nullptr;   // store_mail_out must be set before calling the operator
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
        return std::make_unique<mozi::mail::mo_mail_s>();
    }
};

class mo_mail_translator_c : public mozi::ring::mo_event_translator_c<mo_mail_translator_c, mo_mail_s>
{
  public:
    void operator()(mo_mail_s *event, [[maybe_unused]] size_t sequence) noexcept
    {
#ifndef NDEBUG
        spdlog::debug("mo_mail_translator_c::operator()");
#endif
        event->store_mail_in(bytes, data);
        event->set_behavior(f);
    }
    explicit mo_mail_translator_c(uint8_t *bytes, void *data, void (*f)(uint8_t *buffer, void *data)) noexcept //
        : bytes(bytes),                                                                                        //
          data(data),                                                                                          //
          f(f)                                                                                                 //
    {
    }

    mo_mail_translator_c &operator=(const mo_mail_translator_c &) = delete;
    mo_mail_translator_c &operator=(mo_mail_translator_c &&) = delete;
    mo_mail_translator_c(const mo_mail_translator_c &) = delete;
    mo_mail_translator_c(mo_mail_translator_c &&) = delete;
    ~mo_mail_translator_c() = default;

  private:
    uint8_t *bytes;
    void *data;
    void (*f)(uint8_t *buffer, void *data);
};

struct mo_mail_read_s
{
  public:
    bool on_event(mo_mail_s *event, [[MO_UNUSED]] size_t sequence, [[MO_UNUSED]] bool end_of_batch) noexcept
    {
        // TODO: 完善 增加日志 增加执行结果处理 增加异常处理
        (*event)();
        return true;
    }
};
// struct mo_mail_async_read_s
// {
//   public:

// }
} // namespace mozi::mail