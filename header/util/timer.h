#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include <stdint.h>

#ifdef _MSC_VER
	#include <windows.h>
#elif __clang__
	#include <time.h>
#endif // COMPILER_SELECTION

#ifdef _MSC_VER
	#define TIMER_BEGIN() \
		{ \
			LARGE_INTEGER freq; \
			LARGE_INTEGER begin; \
			QueryPerformanceFrequency(&freq); \
			QueryPerformanceCounter(&begin)
#elif __clang__
	#define TIMER_BEGIN() \
		{ \
			struct timespec begin; \
			struct timespec end; \
			clock_gettime(CLOCK_MONOTONIC, &begin)
#endif // COMPILER_SELECTION

#ifdef _MSC_VER
	#define TIMER_END() \
			LARGE_INTEGER end; \
			QueryPerformanceCounter(&end); \
			int64_t elapsed_ns = ((int64_t)(end.QuadPart) * 1000000000LL - ((int64_t)begin.QuadPart) * 1000000000LL) / ((int64_t)freq.QuadPart); \
			int64_t elapsed_us = elapsed_ns / 1000LL; \
			int64_t elapsed_ms = elapsed_ns / 1000000LL; \
			printf("%s:\n", __func__); \
			printf("\t[ELAPSED] %zdns\n", elapsed_ns); \
			printf("\t[ELAPSED] %zdus\n", elapsed_us); \
			printf("\t[ELAPSED] %zdms\n", elapsed_ms); \
		}
#elif __clang__
	#define TIMER_END() \
			clock_gettime(CLOCK_MONOTONIC, &end); \
			int64_t elapsed_ns = (end.tv_sec - begin.tv_sec) * 1000000000LL + (end.tv_nsec - begin.tv_nsec); \
			int64_t elapsed_us = elapsed_ns / 1000LL; \
			int64_t elapsed_ms = elapsed_ns / 1000000LL; \
			printf("%s:\n", __func__); \
			printf("\t[ELAPSED] %zdns\n", elapsed_ns); \
			printf("\t[ELAPSED] %zdus\n", elapsed_us); \
			printf("\t[ELAPSED] %zdms\n", elapsed_ms); \
		}
#elif __clang__

#endif // COMPILER_SELECTION

#endif // TIMER_H
