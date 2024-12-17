#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

extern void heap_prologue(void);
extern void* heap_alloc(uint64_t size);
extern void* heap_realloc(void* block, uint64_t size);
extern void heap_free(void* block);
extern void heap_epilogue(void);

#ifdef HEAP_IMPLEMENTATION
	static uint64_t s_heap_allocated_bytes;
#endif // HEAP_IMPLEMENTATION

#ifdef HEAP_IMPLEMENTATION
	void heap_prologue(void)
	{
		s_heap_allocated_bytes = 0;
	}
	void* heap_alloc(uint64_t size)
	{
#ifdef _DEBUG
		uint64_t new_size = sizeof(uint64_t) + size;
		uint64_t* new_block = (uint64_t*)malloc(new_size);
		s_heap_allocated_bytes += new_size;
		*new_block = new_size;
		new_block++;
		return new_block;
#else
		return malloc(size);
#endif // _DEBUG
	}
	void* heap_realloc(void* block, uint64_t size)
	{
#ifdef _DEBUG
		if (block)
		{
			uint64_t* old_block = (uint64_t*)block;
			old_block--;
			uint64_t old_size = *old_block;
			uint64_t new_size = sizeof(uint64_t) + size;
			s_heap_allocated_bytes -= old_size;
			uint64_t* new_block = (uint64_t*)realloc(old_block, new_size);
			s_heap_allocated_bytes += new_size;
			*new_block = new_size;
			new_block++;
			return new_block;
		}
		else
		{
			return heap_alloc(size);
		}
#else
		return realloc(block, size);
#endif // _DEBUG
	}
	void heap_free(void* block)
	{
#ifdef _DEBUG
		uint64_t* old_block = (uint64_t*)block;
		old_block--;
		uint64_t old_size = *old_block;
		s_heap_allocated_bytes -= old_size;
		free(old_block);
#else
		free(block);
#endif // _DEBUG
	}
	void heap_epilogue(void)
	{
#ifdef _DEBUG
		if (s_heap_allocated_bytes)
		{
			printf("%zu bytes not freed\n", s_heap_allocated_bytes);
		}
#endif // _DEBUG
		assert(s_heap_allocated_bytes == 0);
	}
#endif // HEAP_IMPLEMENTATION

#endif // HEAP_H
