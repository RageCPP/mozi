#pragma once

#include "../compile/attributes_cpp.hpp"
#include "../nid/worker_config.hpp"
#include "../utils/chrono.hpp"
#include "../utils/exception.hpp"
#include "../variables/mask.hpp"
#include <cstdint>

namespace mozi::nid
{
class mo_nid_worker_c
{
  public:
    explicit mo_nid_worker_c(int64_t base_time, uint32_t worker_id) noexcept : info(base_time, worker_id)
    {
        // base_time check
        auto min_timestamp = mozi::chrono::raw::timestamp(mozi::chrono::raw::unsafe_time(1970, 1, 1));
        auto now_timestamp = mozi::chrono::raw::timestamp(mozi::chrono::raw::now());
        if (base_time < min_timestamp || base_time > now_timestamp)
        {
            throw_exception(
                std::invalid_argument("base_time must be greater than 1970-01-01 and less than current time"));
            return;
        }
        // worker id check
        if (worker_id > mozi::mask::all::mo_5bit)
        {
            throw_exception(std::invalid_argument("worker_id must be less than 16"));
            return;
        }
    }
    /// ms id generator max need < 131071
    [[MO_NODISCARD]] int64_t next_id() noexcept
    {
        int64_t current = current_timetick();
        if (current < info.last_timetick) [[MO_UNLIKELY]]
        {
            throw_exception(std::runtime_error("Clock moved backwards. Refusing to generate id for "
                                               "timestamp "
                                               "less than previous timestamp"));
            return 0;
        }
        bool o = !static_cast<bool>(info.last_timetick ^ current);
        info.sequence = static_cast<uint32_t>(o) * (info.sequence + static_cast<uint32_t>(o));
        info.last_timetick = current;
        return (current << info.timestamp_bit_shift) | (info.worker_id << info.worker_id_bit_shift) | (info.sequence);
    }

  private:
    struct mo__nid_worker_info_s;
    using mo__info_t = mo__nid_worker_info_s;
    inline int64_t current_timetick() noexcept
    {

        return mozi::chrono::raw::timestamp(mozi::chrono::raw::now()) - info.base_time;
    };

  private:
    struct mo__nid_worker_info_s : public mo_nid_worker_config_t
    {
        explicit mo__nid_worker_info_s(int64_t base_time, uint32_t worker_id) noexcept
            : mo_nid_worker_config_t{base_time, worker_id}
        {
        }
        uint32_t sequence = 0;
        int64_t last_timetick = 0;
        uint8_t sequence_bits = mozi::mask::len::mo_17bit;
        uint8_t worker_id_bits = mozi::mask::len::mo_5bit;
        uint8_t worker_id_bit_shift = sequence_bits;
        uint8_t timestamp_bit_shift = sequence_bits + worker_id_bits;
    };
    mo__info_t info;
};
using mo_nid_worker_t = mo_nid_worker_c;
} // namespace mozi::nid