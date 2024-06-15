#pragma once
#include <cstdint>
namespace mozi::mail
{
struct mo_mail_s
{
  public:
    void operator()() noexcept
    {
        m_behavior(m_bytes, m_data);
    };
    void set_behavior(void (*behavior)(uint8_t *, void *)) noexcept
    {
        m_behavior = behavior;
    };

  private:
    void (*m_behavior)(uint8_t *, void *) = nullptr; // behavior must be set before calling the operator
    uint8_t *m_bytes = nullptr;
    void *m_data = nullptr;
};
} // namespace mozi::mail