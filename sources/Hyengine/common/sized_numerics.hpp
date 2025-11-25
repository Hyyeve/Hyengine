#pragma once
#include <atomic>

namespace hyengine
{
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef int8_t i8;
    typedef int16_t i16;
    typedef int32_t i32;
    typedef int64_t i64;

    typedef char char8;
    typedef wchar_t char16;

    typedef char8_t utf8;
    typedef char16_t utf16;
    typedef char32_t utf32;

    typedef float f32;
    typedef double f64;

    typedef std::atomic_bool atomic_bool;

    typedef std::atomic_uint8_t atomic_u8;
    typedef std::atomic_uint16_t atomic_u16;
    typedef std::atomic_uint32_t atomic_u32;
    typedef std::atomic_uint64_t atomic_u64;

    typedef std::atomic_int8_t atomic_i8;
    typedef std::atomic_int16_t atomic_i16;
    typedef std::atomic_int32_t atomic_i32;
    typedef std::atomic_int64_t atomic_i64;
}
