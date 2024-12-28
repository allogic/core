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

# Common libraries
LIB += msvcrt.lib

# Source files
SRC += src/dat.c
SRC += src/ecs.c
SRC += src/heap.c
SRC += src/map.c
SRC += src/set.c
SRC += src/str.c
SRC += src/vec.c
OBJ = $(SRC:.c=.obj)

.PHONY: all always core.lib

all: core.lib

src/%.obj: src/%.c always
	$(CC) -c $< -o $@ $(CFLAGS)

core.lib: $(OBJ)
	$(AR) rcs $@ $(OBJ)
