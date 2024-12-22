#ifndef TST_DAT_H
#define TST_DAT_H

#include "test.h"
#include "heap.h"
#include "dat.h"

static void tst_dat_create_delete(void)
{
	TEST_BEGIN();
	TEST_END();
}
static void tst_dat(void)
{
	test_prologue();
	heap_prologue();
	tst_dat_create_delete();
	heap_epilogue();
	test_epilogue("dat");
}

#endif // TST_DAT_H
