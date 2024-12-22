#define TEST_IMPLEMENTATION
#include "test.h"

#define HEAP_IMPLEMENTATION
#include "heap.h"

#define STR_IMPLEMENTATION
#include "str.h"

#define VEC_IMPLEMENTATION
#include "vec.h"

#define SET_IMPLEMENTATION
#include "set.h"

#define MAP_IMPLEMENTATION
#include "map.h"

#define DAT_IMPLEMENTATION
#include "dat.h"

#define ECS_IMPLEMENTATION
#include "ecs.h"

#include "tst_str.h"
#include "tst_vec.h"
#include "tst_set.h"
#include "tst_map.h"
#include "tst_dat.h"
#include "tst_ecs.h"

int32_t main(void)
{
    tst_str();
    tst_vec();
    tst_set();
    tst_map();
    tst_dat();
	tst_ecs();
	return 0;
}
