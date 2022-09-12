#ifndef H_LOG_H
#define H_LOG_H

#include "Config.h"
#if defined(__cplusplus)
#include "util/Time.hpp"
#endif // defined(__cplusplus)

#if HR_TARGET_GLFW
#include <stdio.h>
#elif HR_TARGET_ANDROID
#include <android/log.h>
#endif // HR_TARGET_GLFW + HR_TARGET_ANDROID

#define _EXPAND(x) x
#define _VARGS(_9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N

//-------------------------------------------------------------------------------------------------
//	Logging
//-------------------------------------------------------------------------------------------------
#if ENABLE_LOGGING

#if HR_TARGET_GLFW

#define _LOG1(format)                                                                             \
do                                                                                                \
{                                                                                                 \
	fprintf(stdout, "[INFO][%s:%d][%s] ", __FILE__, __LINE__, __FUNCTION__);                      \
	fprintf(stdout, format);                                                                      \
	fprintf(stdout, "\n");                                                                        \
} while(0)

#define _LOG2(format, ...)                                                                        \
do                                                                                                \
{                                                                                                 \
	fprintf(stdout, "[INFO][%s:%d][%s] ", __FILE__, __LINE__, __FUNCTION__);                      \
	fprintf(stdout, format, __VA_ARGS__);                                                         \
	fprintf(stdout, "\n");                                                                        \
} while(0)

#elif HR_TARGET_ANDROID

#define _LOG1(format)                                                                             \
do                                                                                                \
{                                                                                                 \
	__android_log_print(ANDROID_LOG_INFO, "harrax", "[INFO][%s:%d][%s] " format, __FILE__,        \
	__LINE__, __FUNCTION__);                                                                      \
} while(0)

#define _LOG2(format, ...)                                                                        \
do                                                                                                \
{                                                                                                 \
	__android_log_print(ANDROID_LOG_INFO, "harrax", "[INFO][%s:%d][%s] " format, __FILE__,        \
	__LINE__, __FUNCTION__, __VA_ARGS__);                                                         \
} while(0)

#endif // HR_TARGET_GLFW + HR_TARGET_ANDROID

#define _LOG_CHOOSER(...) _EXPAND(                                                                \
_VARGS(__VA_ARGS__,                                                                               \
_LOG2, _LOG2, _LOG2,                                                                              \
_LOG2, _LOG2, _LOG2,                                                                              \
_LOG2, _LOG2, _LOG1)                                                                              \
)

#define LOG(...) _EXPAND(_LOG_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#if defined(__cplusplus)
#define LOG_EVERY(period, ...)                                                                    \
{                                                                                                 \
	static double s_LastLogTime_##period = Time::Millis();                                        \
	double s_ThisLogTime_##period = Time::Millis();                                               \
	if (s_ThisLogTime_##period - s_LastLogTime_##period >= period)                                \
	{                                                                                             \
		LOG(__VA_ARGS__);                                                                         \
		s_LastLogTime_##period = s_ThisLogTime_##period;                                          \
	}                                                                                             \
}
#endif // defined(__cplusplus)

#else

#define LOG(...)
#define LOG_EVERY(...)

#endif // ENABLE_LOGGING

//-------------------------------------------------------------------------------------------------
//	Assertions
//-------------------------------------------------------------------------------------------------
#if ENABLE_ASSERTIONS

#if HR_TARGET_GLFW

#define _ASSERT1(condition)                                                                       \
do                                                                                                \
{                                                                                                 \
	if (!(condition))                                                                             \
	{                                                                                             \
		fprintf(stdout, "[ERROR][%s:%d][%s]", __FILE__, __LINE__, __FUNCTION__);                  \
		fprintf(stdout, "\n");                                                                    \
	}                                                                                             \
} while(0)

#define _ASSERT2(condition, format)                                                               \
do                                                                                                \
{                                                                                                 \
	if (!(condition))                                                                             \
	{                                                                                             \
		fprintf(stdout, "[ERROR][%s:%d][%s] ", __FILE__, __LINE__, __FUNCTION__);                 \
		fprintf(stdout, format);                                                                  \
		fprintf(stdout, "\n");                                                                    \
	}                                                                                             \
} while (0)

#define _ASSERT3(condition, format, ...)                                                          \
do                                                                                                \
{                                                                                                 \
	if (!(condition))                                                                             \
	{                                                                                             \
		fprintf(stdout, "[ERROR][%s:%d][%s] ", __FILE__, __LINE__, __FUNCTION__);                 \
		fprintf(stdout, format, __VA_ARGS__);                                                     \
		fprintf(stdout, "\n");                                                                    \
	}                                                                                             \
} while (0)

#elif HR_TARGET_ANDROID

#define _ASSERT1(condition)                                                                       \
do                                                                                                \
{                                                                                                 \
	if (!(condition))                                                                             \
	{                                                                                             \
		__android_log_print(ANDROID_LOG_ERROR, "harrax", "[ERROR][%s:%d][%s]", __FILE__,          \
		__LINE__, __FUNCTION__);                                                                  \
	}                                                                                             \
} while(0)

#define _ASSERT2(condition, format)                                                               \
do                                                                                                \
{                                                                                                 \
	if (!(condition))                                                                             \
	{                                                                                             \
		__android_log_print(ANDROID_LOG_ERROR, "harrax", "[ERROR][%s:%d][%s] " format, __FILE__,  \
		__LINE__,  __FUNCTION__);                                                                 \
	}                                                                                             \
} while (0)

#define _ASSERT3(condition, format, ...)                                                          \
do                                                                                                \
{                                                                                                 \
	if (!(condition))                                                                             \
	{                                                                                             \
		__android_log_print(ANDROID_LOG_ERROR, "harrax", "[ERROR][%s:%d][%s] " format, __FILE__,  \
		__LINE__, __FUNCTION__, __VA_ARGS__);                                                     \
	}                                                                                             \
} while (0)

#endif // HR_TARGET_GLFW + HR_TARGET_ANDROID

#define _ASSERT_CHOOSER(...) _EXPAND(                                                             \
_VARGS(__VA_ARGS__,                                                                               \
_ASSERT3, _ASSERT3, _ASSERT3,                                                                     \
_ASSERT3, _ASSERT3, _ASSERT3,                                                                     \
_ASSERT3, _ASSERT2, _ASSERT1)                                                                     \
)

#define ASSERT(...) _EXPAND(_ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#else

#define ASSERT(condition, ...)                                                                    \
do                                                                                                \
{                                                                                                 \
	if ((condition)) {}                                                                           \
} while (0)

#endif  // ENABLE_ASSERTIONS

#endif // H_LOG_H