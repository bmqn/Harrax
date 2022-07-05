#ifndef H_LOG_H
#define H_LOG_H

#include "Config.h"

#include <stdio.h>

#define _EXPAND(x) x
#define _VARGS(_9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N

//-------------------------------------------------------------------------------------------------
//	Logging
//-------------------------------------------------------------------------------------------------
#if ENABLE_LOGGING

#define _LOG1(format)                                          \
do                                                             \
{                                                              \
	fprintf(stdout, "[INFO][%s:%d][%s] ", __FILE__, __LINE__, __FUNCTION__); \
	fprintf(stdout, format);                                   \
	fprintf(stdout, "\n");                                     \
} while(0)

#define _LOG2(format, ...)                                     \
do                                                             \
{                                                              \
	fprintf(stdout, "[INFO][%s:%d][%s] ", __FILE__, __LINE__, __FUNCTION__); \
	fprintf(stdout, format, __VA_ARGS__);                      \
	fprintf(stdout, "\n");                                     \
} while(0)

#define _LOG_CHOOSER(...) _EXPAND(                             \
_VARGS(__VA_ARGS__,                                            \
_LOG2, _LOG2, _LOG2,                                           \
_LOG2, _LOG2, _LOG2,                                           \
_LOG2, _LOG2, _LOG1)                                           \
)

#define LOG(...) _EXPAND(_LOG_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#else

#define LOG(...)

#endif // LOGGING

//-------------------------------------------------------------------------------------------------
//	Assertions
//-------------------------------------------------------------------------------------------------
#if ENABLE_ASSERTIONS

#define _ASSERT1(condition)                                    \
do                                                             \
{                                                              \
	if (!(condition))                                          \
	{                                                          \
		fprintf(stdout, "[ERROR][%s:%d][%s]", __FILE__, __LINE__, __FUNCTION__); \
		fprintf(stdout, "\n");                                 \
	}                                                          \
} while(0)

#define _ASSERT2(condition, format)                            \
do                                                             \
{                                                              \
	if (!(condition))                                          \
	{                                                          \
		fprintf(stdout, "[ERROR][%s:%d][%s] ", __FILE__, __LINE__, __FUNCTION__); \
		fprintf(stdout, format);                               \
		fprintf(stdout, "\n");                                 \
	}                                                          \
} while (0)

#define _ASSERT3(condition, format, ...)                       \
do                                                             \
{                                                              \
	if (!(condition))                                          \
	{                                                          \
		fprintf(stdout, "[ERROR][%s:%d][%s] ", __FILE__, __LINE__, __FUNCTION__); \
		fprintf(stdout, format, __VA_ARGS__);                  \
		fprintf(stdout, "\n");                                 \
	}                                                          \
} while (0)

#define _ASSERT_CHOOSER(...) _EXPAND(                          \
_VARGS(__VA_ARGS__,                                            \
_ASSERT3, _ASSERT3, _ASSERT3,                                  \
_ASSERT3, _ASSERT3, _ASSERT3,                                  \
_ASSERT3, _ASSERT2, _ASSERT1)                                  \
)

#define ASSERT(...) _EXPAND(_ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#else

#define ASSERT(...)                                            \
do                                                             \
{                                                              \
	if ((condition)) {}                                        \
} while (0)

#endif  // ENABLE_ASSERTIONS

#endif // H_LOG_H
