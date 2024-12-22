CC = clang
RM = rm -rf

BUILD ?= release

CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Iheader
CFLAGS += -Iutil
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

all: tests benchmarks

clean:
	$(RM) tests benchmarks

tests: test/test.c
	$(CC) -o $@ $(CFLAGS) $<
	./tests

benchmarks: benchmark/benchmark.c
	$(CC) -o $@ $(CFLAGS) $<
	./benchmarks
