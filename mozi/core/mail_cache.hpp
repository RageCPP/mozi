#pragma once
#include "mozi/core/mail.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include <memory>

// #include <cstddef>
namespace mozi::mail
{
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

// using mo_mail_cache_t = mo_ring_buffer_data_t<mo_mail_t, 512, ring::mo_single_producer_sequencer_t<>>;
// std::unique_ptr<>
} // namespace mozi::mail