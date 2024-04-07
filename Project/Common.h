#pragma once

#include <cstdint>
#include <cassert>
#include <cstdlib>

#include <string>

// ds
#include <unordered_map>

// util
#include <algorithm>
#include <typeinfo> // typeid

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef int int32;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned int uint;
typedef unsigned long ulong;


#if defined(__WIN32__)
typedef long long int64;
typedef unsigned long long uint64;
#define LV_ALIGN_BEGIN(_align) __declspec(align(_align))
#define LV_ALIGN_END(_align)
#else
#ifdef __PLATFORM32__
typedef long int64;
typedef unsigned long uint64;
#else
typedef long long int64;
typedef unsigned long long uint64;
#endif
#define LV_ALIGN_BEGIN(_align) 
#define LV_ALIGN_END(_align) __attribute__( (aligned(_align) ) )
#endif

#ifdef __PLATFORM64__
typedef uint64 uintptr;
#else
typedef uint32 uintptr;
#endif