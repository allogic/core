#include "test.h"
#include "heap.h"
#include "dat.h"

#include "test_dat.h"

void test_dat_create_delete(void)
{
	TEST_BEGIN();
	// TODO
	TEST_END();
}
void test_dat(void)
{
	heap_prologue();
	test_prologue();
	test_dat_create_delete();
	test_epilogue("dat");
	heap_epilogue();
}
