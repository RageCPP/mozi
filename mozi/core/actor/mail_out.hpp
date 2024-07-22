#pragma once

#include <concepts>
#include <cstdint>

namespace mozi::actor
{
struct mo_mail_out_s
{
  public:
    template <typename F>
        requires std::invocable<const F &&, void *, int64_t, uint16_t> &&
                 std::is_nothrow_invocable_v<const F &&, void *, int64_t, uint16_t>
    void update_data(F update)
    {
        update(m_data, m_start, m_wait_senconds);
    }

    void destroy() noexcept
    {
        if (m_destroy != nullptr)
        {
            m_destroy(m_data);
        }
    }
    ~mo_mail_out_s()
    {
        destroy();
    }
    mo_mail_out_s &operator=(const mo_mail_out_s &) = delete;
    mo_mail_out_s &operator=(mo_mail_out_s &&) = delete;
    mo_mail_out_s(const mo_mail_out_s &) = delete;
    mo_mail_out_s(mo_mail_out_s &&) = delete;
    mo_mail_out_s(void (*destroy)(void *) noexcept, //
                  void *data,                       //
                  int64_t start,                    //
                  uint16_t wait_senconds) noexcept  //
        : m_destroy(destroy),                       //
          m_data(data),                             //
          m_start(start),                           //
          m_wait_senconds(wait_senconds)            //
    {
    }

  private:
    void (*m_destroy)(void *) noexcept;
    void *m_data;
    int64_t m_start;
    uint16_t m_wait_senconds;
};
} // namespace mozi::actor