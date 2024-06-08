#pragma once
#if defined(__has_feature)
#define MO_HAS_FEATURE(...) __has_feature(__VA_ARGS__)
#else
#define MO_HAS_FEATURE(...) 0
#endif

/**
 * Macro for marking functions as having public visibility.
 */
#if defined(__GNUC__)
#define MO_EXPORT __attribute__((__visibility__("default")))
#else
#define MO_EXPORT
#endif

// always inline
#ifdef _MSC_VER
#define MO_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
#define MO_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#define MO_ALWAYS_INLINE inline
#endif

// noline
#ifdef _MSC_VER
#define MO_NOINLINE __declspec(noinline)
#elif defined(__HIP_PLATFORM_HCC__)
// HIP software stack defines its own __noinline__ macro.
#define MO_NOINLINE __attribute__((noinline))
#elif defined(__GNUC__)
#define MO_NOINLINE __attribute__((__noinline__))
#else
#define MO_NOINLINE
#endif

// attribute hidden
#if defined(_MSC_VER)
#define MO_ATTR_VISIBILITY_HIDDEN
#elif defined(__GNUC__)
#define MO_ATTR_VISIBILITY_HIDDEN __attribute__((__visibility__("hidden")))
#else
#define MO_ATTR_VISIBILITY_HIDDEN
#endif

// Microsoft ABI version (can be overridden manually if necessary)
#ifndef MO_MICROSOFT_ABI_VER
#ifdef _MSC_VER
#define MO_MICROSOFT_ABI_VER _MSC_VER
#endif
#endif

//  MO_ERASE
//
//  A conceptual attribute/syntax combo for erasing a function from the build
//  artifacts and forcing all call-sites to inline the callee, at least as far
//  as each compiler supports.
//
//  Semantically includes the inline specifier.
#define MO_ERASE MO_ALWAYS_INLINE MO_ATTR_VISIBILITY_HIDDEN

// Generalize warning push/pop.
#if defined(__GNUC__) || defined(__clang__)
// Clang & GCC
#define MO_PUSH_WARNING _Pragma("GCC diagnostic push")
#define MO_POP_WARNING _Pragma("GCC diagnostic pop")
#define MO_GNU_DISABLE_WARNING_INTERNAL2(warningName) #warningName
#define MO_GNU_DISABLE_WARNING(warningName)                                                                            \
    _Pragma(MO_GNU_DISABLE_WARNING_INTERNAL2(GCC diagnostic ignored warningName))
#ifdef __clang__
#define MO_CLANG_DISABLE_WARNING(warningName) MO_GNU_DISABLE_WARNING(warningName)
#define MO_GCC_DISABLE_WARNING(warningName)
#else
#define MO_CLANG_DISABLE_WARNING(warningName)
#define MO_GCC_DISABLE_WARNING(warningName) MO_GNU_DISABLE_WARNING(warningName)
#endif
#define MO_MSVC_DISABLE_WARNING(warningNumber)
#elif defined(_MSC_VER)
#define MO_PUSH_WARNING __pragma(warning(push))
#define MO_POP_WARNING __pragma(warning(pop))
// Disable the GCC warnings.
#define MO_GNU_DISABLE_WARNING(warningName)
#define MO_GCC_DISABLE_WARNING(warningName)
#define MO_CLANG_DISABLE_WARNING(warningName)
#define MO_MSVC_DISABLE_WARNING(warningNumber) __pragma(warning(disable : warningNumber))
#else
#define MO_PUSH_WARNING
#define MO_POP_WARNING
#define MO_GNU_DISABLE_WARNING(warningName)
#define MO_GCC_DISABLE_WARNING(warningName)
#define MO_CLANG_DISABLE_WARNING(warningName)
#define MO_MSVC_DISABLE_WARNING(warningNumber)
#endif
