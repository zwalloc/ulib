#pragma once

#include <cstdint>
#include <cstddef>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef long long llong;
typedef unsigned long long ullong;

typedef ullong uint64;
typedef uint uint32;
typedef ushort uint16;
typedef uchar uint8;

typedef llong int64;
typedef int int32;
typedef short int16;
typedef char int8;

typedef uchar *rawptr_t;

#ifdef __cpp_char8_t
        typedef char8_t char8;
#else
        typedef char char8;
#endif

typedef char16_t char16;
typedef char32_t char32;
