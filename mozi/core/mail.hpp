#pragma once
#include "spdlog/spdlog.h"
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
    ~mo_mail_s()
    {
        spdlog::info("mo_mail_s::~mo_mail_s()");
        delete[] m_bytes;
    };

  private:
    void (*m_behavior)(uint8_t *, void *) = nullptr; // behavior must be set before calling the operator
    uint8_t *m_bytes = nullptr;
    void *m_data = nullptr;
};
using mo_mail_t = mo_mail_s;
} // namespace mozi::mail