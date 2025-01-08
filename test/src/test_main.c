#include <stdint.h>

#include "test_str.h"
#include "test_vec.h"
#include "test_set.h"
#include "test_map.h"
#include "test_dat.h"
#include "test_ecs.h"

int32_t main(void)
{
    test_str();
    test_vec();
    test_set();
    test_map();
    test_dat();
	test_ecs();
	return 0;
}
