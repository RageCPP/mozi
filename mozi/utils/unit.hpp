#pragma once

namespace mozi
{
/// In functional programming, the degenerate case is often called "unit". In
/// C++, "void" is often the best analogue. However, because of the syntactic
/// special-casing required for void, it is frequently a liability for template
/// metaprogramming. So, instead of writing specializations to handle cases like
/// SomeContainer<void>, a library author may instead rule that out and simply
/// have library users use SomeContainer<Unit>. Contained values may be ignored.
/// Much easier.
///
/// "void" is the type that admits of no values at all. It is not possible to
/// construct a value of this type.
/// "unit" is the type that admits of precisely one unique value. It is
/// possible to construct a value of this type, but it is always the same value
/// every time, so it is uninteresting.
struct Unit
{
    constexpr bool operator==(const Unit & /*other*/) const
    {
        return true;
    }
    constexpr bool operator!=(const Unit & /*other*/) const
    {
        return false;
    }
};

constexpr Unit unit{};
} // namespace mozi
