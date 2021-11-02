#pragma once

#ifdef FL_DEBUG
#ifdef __APPLE__
#define FL_DEBUGBREAK() abort()
#elif defined(WIN32)
#define FL_DEBUGBREAK() __debugbreak()
#endif
#else
#define FL_DEBUGBREAK()
#endif


#ifdef FL_DEBUG
#include "fby_logger/fby_logger.h"

#define FL_LOG(...) fby_logger::log(__VA_ARGS__)
#define FL_DO_ON_ASSERT(x, ...) if(!(x)) __VA_ARGS__;
#define FL_ASSERT(x, ...) FL_DO_ON_ASSERT(x, printf("Assertion Failed: "), fby_logger::log(__VA_ARGS__), fby_logger::log("File: {0}\nLine: {1}\nFunction: {2}()", __FILE__, __LINE__, __FUNCTION__), FL_DEBUGBREAK())
#define FL_BASIC_ASSERT(x) FL_DO_ON_ASSERT(x, fby_logger::log("Assertion Failed: "), fby_logger::log("File: {0}\nLine: {1}\nFunction: {2}()", __FILE__, __LINE__, __FUNCTION__), FL_DEBUGBREAK())
#define FL_ERROR(...) FL_DO_ON_ASSERT(0, fby_logger::log(__VA_ARGS__), FL_DEBUGBREAK())

#elif defined(FL_RELEASE)

#define FL_LOG(...)
#define FL_DO_ON_ASSERT(x, ...)
#define FL_ASSERT(x, ...) if (!(x));
#define FL_BASIC_ASSERT(x) if (!(x));
#define FL_ERROR(...)

#endif
