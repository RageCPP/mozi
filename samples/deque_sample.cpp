#include <atomic>
#include <mozi/core/deque.hpp>
#include <thread>

int main()
{
    // work-stealing queue of integer items
    mozi::mo_deque_c<int> queue;
    std::atomic<bool> begin_steal = false;
    // only one thread can push and pop
    std::thread owner([&]() {
        // TODO 这里如果改为 1'000'000'000 会导致程序卡死
        for (int i = 0; i < 1'000'000; i = i + 1)
        {
            queue.push(i);
        }
        fmt::println("queue size {}", queue.size());
        begin_steal = true;
        int mi = 0;
        while (!queue.empty())
        {
            [[maybe_unused]] std::optional<int> item = queue.pop();
            if (item.has_value())
            {
                mi++;
            }
        }
        fmt::print("mi {}\n", mi);
    });

    // multiple thives can steal
    std::thread thief([&]() {
        while (!begin_steal.load())
        {
            std::this_thread::yield();
        }
        fmt::println("begin stealing");
        int mi = 0;
        while (!queue.empty())
        {
            [[maybe_unused]] std::optional<int> item = queue.steal();
            if (item.has_value())
            {
                mi++;
            }
        }
        fmt::println("mi {}\n", mi);
    });

    owner.join();
    thief.join();

    fmt::println("queue size {}", queue.size());
    return 0;
}