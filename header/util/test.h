#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdint.h>

#ifdef TEST_VERBOSE
	#define TEST_BEGIN() printf("%s:\n", __func__)
#else
	#define TEST_BEGIN()
#endif // TEST_VERBOSE

#ifdef TEST_VERBOSE
	#define TEST_LOG(FMT, ...) printf(FMT, __VA_ARGS__)
#else
	#define TEST_LOG(FMT, ...)
#endif // TEST_VERBOSE

#define TEST_INT_EQUALS(EXPR, EXPECTED) \
	{ \
		int64_t v = (EXPR); \
		uint64_t p = v == (EXPECTED); \
		g_tests_total++; \
		if (p) \
		{ \
			g_tests_passed++; \
		} \
		else \
		{ \
			g_tests_failed++; \
		} \
		TEST_LOG("\t[%s] " #EXPR " (%zd == " #EXPECTED ")\n", (p ? "PASSED" : "ERROR"), v); \
	}

#define TEST_STR_EQUALS(EXPR, EXPECTED) \
	{ \
		char const* v = (EXPR); \
		uint64_t p = strcmp(v, (EXPECTED)) == 0; \
		g_tests_total++; \
		if (p) \
		{ \
			g_tests_passed++; \
		} \
		else \
		{ \
			g_tests_failed++; \
		} \
		TEST_LOG("\t[%s] " #EXPR " (\"%s\" == \"%s\")\n", (p ? "PASSED" : "ERROR"), (strlen(v) ? v : ""), (strlen(EXPECTED) ? EXPECTED : "")); \
	}

#ifdef TEST_VERBOSE
	#define TEST_END()
#else
	#define TEST_END()
#endif // TEST_VERBOSE

extern void test_prologue(void);
extern void test_epilogue(void);

#ifdef TEST_IMPLEMENTATION
	uint64_t g_tests_total;
	uint64_t g_tests_passed;
	uint64_t g_tests_failed;
#else
	extern uint64_t g_tests_total;
	extern uint64_t g_tests_passed;
	extern uint64_t g_tests_failed;
#endif // TEST_IMPLEMENTATION

#ifdef TEST_IMPLEMENTATION
	void test_prologue(void)
	{
		g_tests_total = 0;
		g_tests_passed = 0;
		g_tests_failed = 0;
	}
	void test_epilogue(void)
	{
		printf("test_stats:\n");
		printf("\ttotal: %zu\n", g_tests_total);
		printf("\tpassed: %zu\n", g_tests_passed);
		printf("\tfailed: %zu\n", g_tests_failed);
	}
#endif // TEST_IMPLEMENTATION

#endif // TEST_H