#pragma once

#include <cstdint>

namespace mozi::mail
{
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
} // namespace mozi::mail