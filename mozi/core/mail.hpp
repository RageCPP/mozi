#pragma once
#include "mozi/compile/attributes_cpp.hpp"
#include "mozi/core/ring/event_factory.hpp"
#include "mozi/core/ring/event_translator.hpp"
#include "spdlog/spdlog.h"
#include <cstddef>
#include <cstdint>
namespace mozi::mail
{
// TODO 这里MMU 寻址优化
// 增加 void *data 的内存对齐
struct mo_mail_s
{
  public:
    void operator()() noexcept
    {
        m_behavior(m_bytes, m_data);
    }
    ~mo_mail_s()
    {
        spdlog::info("mo_mail_s::~mo_mail_s()");
        delete[] m_bytes;
        free(m_data);
    }
    friend class mo_mail_translator_c;

  private:
    void update_mail(uint8_t *bytes, void *data)
    {
        if (m_bytes != nullptr) [[MO_LIKELY]]
        {
            delete[] m_bytes;
        }
        free(m_data);
        m_bytes = bytes;
        m_data = data;
    }
    void set_behavior(void (*behavior)(uint8_t *, void *)) noexcept
    {
        m_behavior = behavior;
    }
    void (*m_behavior)(uint8_t *, void *) = nullptr; // behavior must be set before calling the operator
    uint8_t *m_bytes = nullptr;
    void *m_data = nullptr;
};
using mo_mail_t = mo_mail_s;

class mo_mail_factory_c : public mozi::ring::mo_event_factory_c<mo_mail_factory_c, mo_mail_t>
{
  public:
    mo_mail_factory_c() = default;
    [[MO_NODISCARD]] static mo_mail_t create_instance() noexcept
    {
        return mozi::mail::mo_mail_s{};
    }
};
using mo_mail_factory_t = mo_mail_factory_c;

class mo_mail_translator_c : public mozi::ring::mo_event_translator_c<mo_mail_translator_c, mo_mail_t>
{
  public:
    void operator()(mo_mail_t &event, [[maybe_unused]] size_t sequence) noexcept
    {
        event.update_mail(bytes, data);
        event.set_behavior(f);
    }
    mo_mail_translator_c(uint8_t *bytes, void *data, void (*f)(uint8_t *buffer, void *data)) noexcept
        : bytes(bytes), data(data), f(f)
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
using mo_mail_translator_t = mo_mail_translator_c;

class mo_mail_read_c
{
  public:
    bool on_event(mo_mail_t &event, [[maybe_unused]] size_t sequence, [[maybe_unused]] bool end_of_batch) noexcept
    {
        spdlog::info("mo_mail_read_c::on_event");
        event();
        return true;
    }
};
using mo_mail_read_t = mo_mail_read_c;
} // namespace mozi::mail