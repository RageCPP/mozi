#pragma once

#include "mozi/core/future.hpp"
#include "mozi/core/mail_out.hpp"
#include <cstdint>
#include <cstdlib>

namespace mozi::mail
{
struct mo_async_mail_s
{
  public:
  private:
    void update_mail(uint8_t *serial_mail_in, void *mail_in, mo_mail_out_s *mail_out) noexcept
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
    }
    coro::mo_future_s (*m_behavior)(uint8_t *, void *, mo_mail_out_s *) = nullptr;
    uint8_t *m_serial_in = nullptr;
    void *m_in = nullptr;
    mo_mail_out_s *m_out = nullptr;
};
} // namespace mozi::mail