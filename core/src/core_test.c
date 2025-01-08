#include <stdio.h>

#include "core_test.h"

uint64_t g_tests_total;
uint64_t g_tests_passed;
uint64_t g_tests_failed;

void test_prologue(void)
{
	g_tests_total = 0;
	g_tests_passed = 0;
	g_tests_failed = 0;
}
void test_epilogue(char const* name)
{
	printf("test_stats_%s:\n", name);
	printf("\ttotal: %zu\n", g_tests_total);
	printf("\tpassed: %zu\n", g_tests_passed);
	printf("\tfailed: %zu\n", g_tests_failed);
}
