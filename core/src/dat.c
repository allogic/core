#include <stdio.h>
#include <string.h>

#include "dat.h"

#ifdef _HEAP_TRACE
	#include "heap.h"
	#define HEAP_ALLOC(SIZE) heap_alloc(SIZE)
	#define HEAP_REALLOC(BLOCK, SIZE) heap_realloc(BLOCK, SIZE)
	#define HEAP_FREE(BLOCK) heap_free(BLOCK)
#else
	#include <stdlib.h>
	#define HEAP_ALLOC(SIZE) malloc(SIZE)
	#define HEAP_REALLOC(BLOCK, SIZE) realloc(BLOCK, SIZE)
	#define HEAP_FREE(BLOCK) free(BLOCK)
#endif // _HEAP_TRACE

dat_t dat_alloc(uint64_t value_size)
{
	dat_t dat;
	memset(&dat, 0, sizeof(dat_t));
	dat.pages = 0;
	dat.page_size = 0;
	dat.page_count = 0;
	dat.values = vec_alloc(value_size);
	dat.ids = vec_alloc(sizeof(uint64_t));
	return dat;
}
dat_t dat_copy(dat_t* ref)
{
	dat_t dat;
	memset(&dat, 0, sizeof(dat_t));
	dat.pages = (page_t*)HEAP_ALLOC(ref->page_size);
	dat.page_size = ref->page_size;
	dat.page_count = ref->page_count;
	dat.values = vec_copy(&ref->values);
	dat.ids = vec_copy(&ref->ids);
	uint64_t page_index = 0;
	while (page_index < dat.page_count)
	{
		page_t* ref_page = &ref->pages[page_index];
		page_t* new_page = &dat.pages[page_index];
		new_page->sparse = (uint64_t*)HEAP_ALLOC(ref_page->sparse_size);
		new_page->sparse_size = ref_page->sparse_size;
		new_page->sparse_count = ref_page->sparse_count;
		uint64_t sparse_index = 0;
		while (sparse_index < new_page->sparse_count)
		{
			new_page->sparse[sparse_index] = DAT_TOMBSTONE;
			sparse_index++;
		}
		page_index++;
	}
	return dat;
}
uint8_t dat_equal(dat_t* ref)
{
	// TODO
	return 0;
}
void dat_set_dense_index(dat_t* dat, uint64_t id, uint64_t index)
{
	uint64_t page_index = id / DAT_SPARSE_LIMIT;
	uint64_t sparse_index = id % DAT_SPARSE_LIMIT;
	dat_pages_resize(dat, page_index + 1); 
	page_t* page = &dat->pages[page_index];
	dat_sparse_resize(dat, page, sparse_index + 1);
	page->sparse[sparse_index] = index;
}
uint64_t dat_get_dense_index(dat_t* dat, uint64_t id)
{
	uint64_t page_index = id / DAT_SPARSE_LIMIT;
	uint64_t sparse_index = id % DAT_SPARSE_LIMIT;
	if (page_index >= dat->page_count)
	{
		return DAT_TOMBSTONE;
	}
	page_t* page = &dat->pages[page_index];
	if (sparse_index >= page->sparse_count)
	{
		return DAT_TOMBSTONE;
	}
	return page->sparse[sparse_index];
}
void dat_pages_resize(dat_t* dat, uint64_t page_count)
{
	if (page_count > dat->page_count)
	{
		dat->pages = (page_t*)HEAP_REALLOC(dat->pages, page_count * sizeof(page_t));
		uint64_t page_index = dat->page_count;
		while (page_index < page_count)
		{
			page_t* page = &dat->pages[page_index];
			page->sparse = (uint64_t*)HEAP_ALLOC(DAT_SPARSE_COUNT * sizeof(uint64_t));
			page->sparse_size = DAT_SPARSE_COUNT * sizeof(uint64_t);
			page->sparse_count = DAT_SPARSE_COUNT;
			uint64_t sparse_index = 0;
			while (sparse_index < page->sparse_count)
			{
				page->sparse[sparse_index] = DAT_TOMBSTONE;
				sparse_index++;
			}
			page_index++;
		}
		dat->page_size = page_count * sizeof(page_t);
		dat->page_count = page_count;
	}
}
void dat_sparse_resize(dat_t* dat, page_t* page, uint64_t sparse_count)
{
	if (sparse_count > page->sparse_count)
	{
		page->sparse = (uint64_t*)HEAP_REALLOC(page->sparse, sparse_count * sizeof(uint64_t));
		uint64_t sparse_index = page->sparse_count;
		while (sparse_index < sparse_count)
		{
			page->sparse[sparse_index] = DAT_TOMBSTONE;
			sparse_index++;
		}
		page->sparse_size = sparse_count * sizeof(uint64_t);
		page->sparse_count = sparse_count;
	}
}
void dat_set(dat_t* dat, uint64_t id, void const* value)
{
	uint64_t dense_index = dat_get_dense_index(dat, id);
	if (dense_index == DAT_TOMBSTONE)
	{
		uint64_t value_count = vec_count(&dat->values);
		dat_set_dense_index(dat, id, value_count);
		vec_push(&dat->values, value);
		vec_push(&dat->ids, &id);
	}
	else
	{
		vec_set(&dat->values, dense_index, value);
		vec_set(&dat->ids, dense_index, &id);
	}
}
void* dat_get(dat_t* dat, uint64_t id)
{
	uint64_t dense_index = dat_get_dense_index(dat, id);
	if (dense_index == DAT_TOMBSTONE)
	{
		return 0;
	}
	else
	{
		return vec_at(&dat->values, dense_index);
	}
}
void dat_remove(dat_t* dat, uint64_t id)
{
	uint64_t dense_index = dat_get_dense_index(dat, id);
	if (dense_index == DAT_TOMBSTONE)
	{
		return;
	}
	uint64_t value_count = vec_count(&dat->values);
	uint64_t id_count = vec_count(&dat->ids);
	dat_set_dense_index(dat, id_count - 1, dense_index);
	dat_set_dense_index(dat, id, DAT_TOMBSTONE);
	vec_swap(&dat->values, dense_index, value_count - 1);
	vec_swap(&dat->ids, dense_index, id_count - 1);
	vec_pop(&dat->values, 0);
	vec_pop(&dat->ids, 0);
}
uint8_t dat_contains(dat_t* dat, uint64_t id)
{
	return dat_get_dense_index(dat, id) != DAT_TOMBSTONE;
}
uint8_t dat_empty(dat_t* dat)
{
	return vec_empty(&dat->values);
}
void dat_clear(dat_t* dat)
{
	uint64_t page_index = 0;
	while (page_index < dat->page_count)
	{
		page_t* page = &dat->pages[page_index];
		HEAP_FREE(page->sparse);
		page_index++;
	}
	if (dat->pages)
	{
		HEAP_FREE(dat->pages);
		dat->pages = 0;
		dat->page_size = 0;
		dat->page_count = 0;
	}
	vec_clear(&dat->values);
	vec_clear(&dat->ids);
}
uint64_t dat_count(dat_t* dat)
{
	return vec_count(&dat->ids);
}
uint64_t dat_id(dat_t* dat, uint64_t index)
{
	return *(uint64_t*)vec_at(&dat->ids, index);
}
void* dat_value(dat_t* dat, uint64_t index)
{
	return vec_at(&dat->values, index);
}
void dat_print(dat_t* dat)
{
	uint64_t page_index = 0;
	printf("[Direct Access Tree]\n");
	printf("\tPages [%zu]\n", dat->page_count);
	while (page_index < dat->page_count)
	{
		page_t* page = &dat->pages[page_index];
		uint64_t sparse_index = 0;
		printf("\t\tPage [%zu]\n", page->sparse_count);
		while (sparse_index < page->sparse_count)
		{
			printf("\t\t\t[%zu] -> [%zu]\n", sparse_index, page->sparse[sparse_index]);
			sparse_index++;
		}
		printf("\n");
		page_index++;
	}
	uint64_t value_index = 0;
	uint64_t value_count = vec_count(&dat->values);
	printf("\tValues [%zu]\n", value_count);
	while (value_index < value_count)
	{
		printf("\t\t[%zu] -> [%p]\n", value_index, vec_at(&dat->values, value_index));
		value_index++;
	}
	printf("\n");
	uint64_t id_index = 0;
	uint64_t id_count = vec_count(&dat->ids);
	printf("\tIds [%zu]\n", id_count);
	while (id_index < id_count)
	{
		printf("\t\t[%zu] -> [%zu]\n", id_index, *(uint64_t*)vec_at(&dat->ids, id_index));
		id_index++;
	}
	printf("\n");
}
void dat_free(dat_t* dat)
{
	uint64_t page_index = 0;
	while (page_index < dat->page_count)
	{
		page_t* page = &dat->pages[page_index];
		HEAP_FREE(page->sparse);
		page_index++;
	}
	if (dat->pages)
	{
		HEAP_FREE(dat->pages);
	}
	vec_free(&dat->values);
	vec_free(&dat->ids);
	memset(dat, 0, sizeof(dat_t));
}
