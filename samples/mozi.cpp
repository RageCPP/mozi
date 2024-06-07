#include <fmt/core.h>
#include <mozi/utils/expected.hpp>
int main()
{
    mozi::Expected<int, bool> e = 42;
    fmt::println("{}", e.value());
    return 0;
}