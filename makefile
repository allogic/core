CC = clang
RM = rm -rf

BUILD ?= release

CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Iheader
CFLAGS += -Iheader/util
CFLAGS += -Wno-comment
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-declaration-after-statement
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-gnu-binary-literal
CFLAGS += -D_POSIX_C_SOURCE=200809L

ifeq ($(BUILD),debug)
	CFLAGS += -Og
	CFLAGS += -g
	CFLAGS += -D_HEAP_TRACE
	CFLAGS += -D_DEBUG
else ifeq ($(BUILD),release)
	CFLAGS += -O3
	CFLAGS += -march=native
	CFLAGS += -flto
	CFLAGS += -fomit-frame-pointer
endif

TESTS += tst_str
TESTS += tst_vec
TESTS += tst_set
TESTS += tst_map
TESTS += tst_ecs

BENCHMARKS += bm_ecs

all: tests benchmarks

clean:
	$(RM) $(TESTS) $(BENCHMARKS)

tests: $(TESTS)
	./tst_str
	./tst_vec
	./tst_set
	./tst_map
	./tst_ecs

benchmarks: $(BENCHMARKS)
	./bm_ecs

tst_str: test/tst_str.c
	$(CC) -o $@ $(CFLAGS) $<

tst_vec: test/tst_vec.c
	$(CC) -o $@ $(CFLAGS) $<

tst_set: test/tst_set.c
	$(CC) -o $@ $(CFLAGS) $<

tst_map: test/tst_map.c
	$(CC) -o $@ $(CFLAGS) $<

tst_ecs: test/tst_ecs.c
	$(CC) -o $@ $(CFLAGS) $<

bm_ecs: benchmark/bm_ecs.c
	$(CC) -o $@ $(CFLAGS) $<
