BUILD ?= release

# Toolchain
CC = clang.exe
AR = llvm-ar.exe
LD = lld-link.exe

# Common compiler flags
CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Wno-comment
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-declaration-after-statement
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-gnu-binary-literal
CFLAGS += -Wno-c23-extensions
CFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS += -I "./header"
CFLAGS += -I "../core/header"

# Platform specific compiler flags
CFLAGS += -target x86_64-pc-windows-msvc

# Build specific compiler flags
ifeq ($(BUILD), debug)
	CFLAGS += -Og
	CFLAGS += -g
	CFLAGS += -D_DEBUG
	CFLAGS += -D_HEAP_TRACE
else ifeq ($(BUILD), release)
	CFLAGS += -O3
	CFLAGS += -march=native
	CFLAGS += -fomit-frame-pointer
	CFLAGS += -flto
endif

# Common linker flags
LFLAGS += /machine:x64
LFLAGS += /subsystem:console
LFLAGS += /libpath:"../core"

# Common libraries
LIB += msvcrt.lib
LIB += core.lib

# Source files
SRC += src/benchmark_ecs.c
SRC += src/main.c
OBJ = $(SRC:.c=.obj)

.PHONY: all always benchmark.exe

all: benchmark.exe

src/%.obj: src/%.c always
	$(CC) -c $< -o $@ $(CFLAGS)

benchmark.exe: $(OBJ)
	$(LD) $(OBJ) /out:$@ $(LFLAGS) $(LIB)
