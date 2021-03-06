/*
 *      This file is part of the SmokeOS project.
 *  Copyright (C) 2015  <Fabien Bavent>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#include <stdint.h>
#include <errno.h>
#include <stddef.h>
#include <axc/spinlock.h>
#include <axc/llist.h>

#define __axlog(m, ...) ((void)0)


/* Private Macros ---------------------------------------------------------- */
#define ALLOC_MAX_CHUNK     0x30000000
#define ALLOC_MIN_CHUNK     sizeof(struct SMK_HeapChunk)
#define ALLOC_CHUNK_HEAD    (sizeof (uint32_t)*4)
#define alloc_chunk(p)      ((struct SMK_HeapChunk*)((uint32_t)(p) - ALLOC_CHUNK_HEAD))

#define ALLOC_CHECK         0x01
#define ALLOC_ISUSED        0x01
#define ALLOC_CORRUPTED     0x02
#define ALLOC_PARANOID      0x04

#define ALIGN(v,a)      (((v)+(a-1))&(~(a-1)))
#define _LOCK(h)        ((void)(0))
#define _UNLOCK(h)        ((void)(0))

/* Structures definitions -------------------------------------------------- */

struct SMK_HeapChunk {
  uint32_t prev_size;
  uint32_t reserved;
  uint32_t chunk_size;
  uint32_t is_used;
  union {
    // If the block is empty
    struct {
      struct SMK_HeapChunk *prev_chunk;
      struct SMK_HeapChunk *next_chunk;
    };
    // If the block is used
    uint32_t data[4];
  };
};

/** @brief Structure that hold information about a single heap arena
  */
struct SMK_HeapArea {
  struct SMK_HeapChunk *start;
  struct SMK_HeapChunk *free_list;
  size_t max;
  size_t available;
  size_t begin_;
  size_t end_;
  char lock;
  long flags;
  struct spinlock lock_;
  struct llnode node_;
  int (*freed_)(size_t base,size_t length);
};

/** @brief Global list of all defined heap arenas */
struct llhead gHeapArea;

/** @brief Global item used for the first heap arena which can't be allocated
  */
struct SMK_HeapArea gArea;


/* Functions --------------------------------------------------------------- */
#define valloc(n) memalign_r(heap, PAGE_SIZE, (n))
#define pvalloc(n) memalign_r(heap, PAGE_SIZE, ALIGN(n, PAGE_SIZE))



/* ----------------------------------------------------------------------- */
/** @brief Append a memory block on the list of free blocks.
  * @param heap Structure that define the heap arena.
  * @param chunk Pointer on the chunk to add on the free list.
  *
  * The method is a bit time consuming since we keep the block in order of
  * size. The performance is sensible to heap fragmentation.
  */
static void alloc_add_to_free (struct SMK_HeapArea *heap, struct SMK_HeapChunk *chunk)
{
  struct SMK_HeapChunk *curs = heap->free_list;
  chunk->is_used = 0; //  &= ~ALLOC_ISUSED;

  // In case there is no available blocks
  if ( !curs ) {
    heap->free_list = chunk;
    chunk->prev_chunk = NULL;
    chunk->next_chunk = NULL;
    return;
  }

  // In case the free block is the smallest of all
  if ( curs->chunk_size > chunk->chunk_size ) {
    heap->free_list = chunk;
    chunk->prev_chunk = NULL;
    chunk->next_chunk = curs;
    curs->prev_chunk = chunk;
    return;
  }

  // Browse the list until we found a bigger block or the end of the list
  while ( curs->next_chunk ) {
    if ( curs->next_chunk->chunk_size > chunk->chunk_size ) {
      break;
    }

    curs = curs->next_chunk;
  }

  if ( curs->next_chunk ) {
    curs->next_chunk->prev_chunk = chunk;
  }

  chunk->prev_chunk = curs;
  chunk->next_chunk = curs->next_chunk;
  curs->next_chunk = chunk;
}


/* ----------------------------------------------------------------------- */
/** @brief Remove a memory block of the list of free blocks.
  * @param heap Structure that define the heap arena.
  * @param chunk Pointer on the chunk to remove from the free list.
  */
static void alloc_rem_of_free (struct SMK_HeapArea *heap, struct SMK_HeapChunk *chunk)
{
  if ( chunk->next_chunk ) {
    chunk->next_chunk->prev_chunk = chunk->prev_chunk;
  }

  if ( chunk->prev_chunk ) {
    chunk->prev_chunk->next_chunk = chunk->next_chunk;
  } else {
    heap->free_list = chunk->next_chunk;
  }

  chunk->prev_chunk = (struct SMK_HeapChunk *)0xcccccccc;
  chunk->next_chunk = (struct SMK_HeapChunk *)0xcccccccc;
  chunk->is_used |= ALLOC_ISUSED;
}


/* ----------------------------------------------------------------------- */
/** @brief retrace the heap to detect memory corruption
  * @retval ZERO if the heap is not corrupted
  * @retval >0 The number of check that failed.
  *
  * Browse the list of block on address order check consistency and count
  * the number of blocks. Compare that to the free list and check list
  * consistency.
  */
int memcorrupt_r (struct SMK_HeapArea *heap)
{
  int err = 0;
  int free_chunks = 0;
  int total_chunks = 0;
  uintmax_t lsize = 0;
  struct SMK_HeapChunk *chunk = heap->free_list;
  struct SMK_HeapChunk *prev = NULL;

  while (chunk != NULL) {
    free_chunks++;

    if (chunk->is_used) {
      __axlog ("Free chunk at 0x%x mark as used\n", chunk);
      err++;
    }

    if (chunk->prev_chunk != prev) {
      __axlog ("Free Chunk at 0x%x isn't link to previous\n", chunk);
      err++;
    }

    if (chunk->chunk_size < lsize) {
      __axlog ("Free chunk at 0x%x is smaller than previous ones\n", chunk);
      err++;
    }

    lsize = chunk->chunk_size;
    prev = chunk;
    chunk = chunk->next_chunk;
  }

  chunk = heap->start;
  lsize = 0;

  while ((size_t)chunk < heap->max) {
    total_chunks++;

    if (chunk->is_used) {
      /*
      if (chunk->next_chunk != (struct SMK_HeapChunk*)0xcccccccc)
          err++;
      if (chunk->prev_chunk != (struct SMK_HeapChunk*)0xcccccccc)
          err++; */
    } else {
      free_chunks--;

      if ((uint32_t)chunk->next_chunk & 7)
        err++;

      if ((uint32_t)chunk->prev_chunk & 7)
        err++;
    }

    if (chunk->prev_size != lsize) {
      __axlog ("Wrong prev size mark at 0x%x\n", chunk);
      err++;
    }

    lsize = chunk->chunk_size;
    chunk = (struct SMK_HeapChunk *)((size_t)chunk + (size_t)chunk->chunk_size);
  }

  if (free_chunks != 0) {
    __axlog ("Free chunks not referenced\n", chunk);
    err++;
  }

  if ((size_t)chunk != heap->max) {
    __axlog ("Incomplete chunk map\n", chunk);
    err++;
  }

  return (err);
}

/* ----------------------------------------------------------------------- */
/** @brief Initialize a heap segment structure info. */
void meminit_r(struct SMK_HeapArea *heap, void *base, size_t length)
{
}


/* ----------------------------------------------------------------------- */
/** @brief Allocate dynamic memory
  * @return The malloc() and calloc() functions return a pointer to the
  * allocated memory that is suitably aligned for any kind of variable. On
  * error, these functions return NULL. NULL may also be returned by a
  * successful call to malloc() with a size of zero, or by a successful call to
  * calloc() with nmemb or size equal to zero.
  *
  * The malloc() function allocates size bytes and returns a pointer to the
  * allocated memory. The memory is not initialized. If size is 0, then
  * malloc() returns either NULL, or a unique pointer value that can later be
  * successfully passed to free().
  *
  * @see malloc, free, calloc, realloc
  * @see brk, mmap, alloca, malloc_get_state, malloc_info, malloc_trim,
  * malloc_usable_size, mallopt, mcheck, mtrace, posix_memalign
  * @note By default, the kernel follows an optimistic memory allocation
  * strategy. This means that when malloc() returns non-NULL there is no
  * guarantee that the memory really is available. In case it turns out that
  * the system is out of memory, one or more processes will be killed.
  */
void *malloc_r(struct SMK_HeapArea *heap, size_t size)
{
  struct SMK_HeapChunk *chunk = heap->free_list;
  struct SMK_HeapChunk *split = NULL;
  struct SMK_HeapChunk *prev = NULL;
  struct SMK_HeapChunk *next = NULL;
  size_t lsize = 0;

  _LOCK(heap);

  if (heap->flags & ALLOC_PARANOID) {
    if (memcorrupt_r(heap)) {
      __axlog ("HEAP IS CORRUPTED #1\n");
    }
  }

  // We align the length requested
  if (size > ALLOC_MAX_CHUNK - ALLOC_MIN_CHUNK) {
    _UNLOCK(heap);
    return NULL;
  }

  size = ALIGN (size + ALLOC_CHUNK_HEAD, ALLOC_MIN_CHUNK );

  if (size < ALLOC_MIN_CHUNK) size = ALLOC_MIN_CHUNK;

  // Browse the free chunk list
  while ( chunk ) {

    // If we ask for heap corruption checks
    if (heap->flags & ALLOC_CHECK) {
      if (chunk->is_used)
        heap->flags |= ALLOC_CORRUPTED;

      if (chunk->prev_chunk != prev)
        heap->flags |= ALLOC_CORRUPTED;

      if (chunk->chunk_size < lsize)
        heap->flags |= ALLOC_CORRUPTED;

      if (heap->flags & ALLOC_CORRUPTED) {
        _UNLOCK(heap);
        return NULL;
      }

      lsize = chunk->chunk_size;
      prev = chunk;
    }

    if ( chunk->chunk_size >= size) {
      alloc_rem_of_free ( heap, chunk );

      if (chunk->chunk_size >= size + ALLOC_MIN_CHUNK ) {
        // If the size is enough for a new block
        split = ( struct SMK_HeapChunk *) ((( size_t ) chunk ) + size);
        split->chunk_size = chunk->chunk_size - size;
        split->prev_size = size;
        next =  (struct SMK_HeapChunk *)((size_t)split + (size_t)split->chunk_size);

        if ((size_t)next < heap->max)
          next->prev_size = split->chunk_size;
        else if ((heap->flags & ALLOC_CHECK) && (size_t)next != heap->max) {
          heap->flags |= ALLOC_CORRUPTED;
          _UNLOCK(heap);
          return NULL;
        }

        alloc_add_to_free ( heap, split );
        chunk->chunk_size = size;
        chunk->is_used |= ALLOC_ISUSED;
      }

      heap->available -= size;
      _UNLOCK(heap);

      if (heap->flags & ALLOC_PARANOID) {
        if (memcorrupt_r(heap)) {
          __axlog ("HEAP IS CORRUPTED #2\n");
        }
      }

      // __axlog ("MALLOc RETURN 0x%x or 0x%x\n", chunk->data, &chunk->prev_chunk);
      return (chunk->data);
    }

    chunk = chunk->next_chunk;
  }

  _UNLOCK(heap);
  errno = ENOMEM;

  if (heap->flags & ALLOC_PARANOID) {
    if (memcorrupt_r(heap)) {
      __axlog ("HEAP IS CORRUPTED #3\n");
    }
  }

  return NULL;
}


/* ----------------------------------------------------------------------- */
/** @brief Free dynamic memory
  * @return The free() function returns no value.
  *
  * The free() function frees the memory space pointed to by ptr, which must
  * have been returned by a previous call to malloc(), calloc() or realloc().
  * Otherwise, or if free(ptr) has already been called before, undefined
  * behavior occurs. If ptr is NULL, no operation is performed.
  *
  * @see malloc, free, calloc, realloc
  * @see brk, mmap, alloca, malloc_get_state, malloc_info, malloc_trim,
  * malloc_usable_size, mallopt, mcheck, mtrace, posix_memalign
  */
void free_r(struct SMK_HeapArea *heap, void *ptr)
{
  struct SMK_HeapChunk *chunk = alloc_chunk(ptr);
  struct SMK_HeapChunk *prev = NULL;
  struct SMK_HeapChunk *next = NULL;

  if (heap->flags & ALLOC_PARANOID) {
    if (memcorrupt_r(heap)) {
      __axlog ("HEAP IS CORRUPTED #4\n");
    }
  }

  if ((size_t)chunk < (size_t)heap->start || (size_t)chunk > heap->max) {
    return;
  }

  _LOCK(heap);
  prev = (struct SMK_HeapChunk *)((size_t)chunk - (size_t)chunk->prev_size);
  next = (struct SMK_HeapChunk *)((size_t)chunk + (size_t)chunk->chunk_size);

  // If we ask for heap corruption checks
  if (heap->flags & ALLOC_CHECK) {
    if (!chunk->is_used)
      heap->flags |= ALLOC_CORRUPTED;

    if (prev != chunk && prev->chunk_size != chunk->prev_size)
      heap->flags |= ALLOC_CORRUPTED;

    if ((size_t)next < heap->max && next->prev_size != chunk->chunk_size)
      heap->flags |= ALLOC_CORRUPTED;

    if (heap->flags & ALLOC_CORRUPTED) {
      _UNLOCK(heap);

      if (heap->flags & ALLOC_PARANOID) {
        if (memcorrupt_r(heap)) {
          __axlog ("HEAP IS CORRUPTED #5\n");
        }
      }

      return;
    }
  }

  // Increase available memory
  heap->available += chunk->chunk_size;

  // If the previous chunk is unused collapse
  if (prev != chunk && !prev->is_used) {
    alloc_rem_of_free(heap, prev);
    // prev->is_used = FALSE;
    prev->chunk_size += chunk->chunk_size;

    if ((size_t)next < heap->max)
      next->prev_size = prev->chunk_size;

    chunk = prev;
  }

  // If the next chunk is unused collapse
  if ((size_t)next < heap->max && !next->is_used) {
    alloc_rem_of_free(heap, next);
    // next->is_used = FALSE;
    chunk->chunk_size += next->chunk_size;
    next = (struct SMK_HeapChunk *)((size_t)chunk + (size_t)chunk->chunk_size);

    if ((size_t)next < heap->max)
      next->prev_size = chunk->chunk_size;
  }

  // Freed the chunk
  alloc_add_to_free (heap, chunk);

  if (heap->flags & ALLOC_PARANOID) {
    if (memcorrupt_r(heap)) {
      __axlog ("HEAP IS CORRUPTED #6\n");
    }
  }

  _UNLOCK(heap);
}

#if 0
/* ----------------------------------------------------------------------- */
/** @brief Allocate aligned memory
  * @return aligned_alloc(), memalign(), valloc(), and pvalloc() return a
  * pointer to the allocated memory, or NULL if the request fails.
  *
  * The obsolete function memalign() allocates size bytes and returns a pointer
  * to the allocated memory. The memory address will be a multiple of alignment,
  * which must be a power of two.
  *
  * @throw EINVAL - The alignment argument was not a power of two, or was not a
  * multiple of sizeof(void *).
  * @throw ENOMEM - There was insufficient memory to fulfill the allocation
  * request.
  * @see posix_memalign, aligned_alloc, memalign, valloc, pvalloc
  * @see brk, mmap, alloca, malloc_get_state, malloc_info, malloc_trim,
  * malloc_usable_size, mallopt, mcheck, mtrace, posix_memalign
 */
void *memalign_r(struct SMK_HeapArea *heap, size_t alignment, size_t size)
{
  xHeapChunk_t *chunk;
  int *ptr, *aptr;

  if (!bpw2(alignment)) {
    /// @todo errno = EINVAL
    return NULL;
  }

  if (alignment <= 16)
    return (malloc_r(heap, size));

  ptr = (int *)malloc_r(heap, size + alignment);

  if (((size_t)ptr & (alignment - 1)) == 0) {
    // Is already aligned
    chunk = alloc_chunk(ptr);
    //  chunk->chunk_size = ;
  }

  aptr = (int *)ALIGN((uintptr_t)ptr, alignment);



  return (ptr);
}
#endif




/* ----------------------------------------------------------------------- */
/** @brief Allocate dynamic memory
  * @return The malloc() and calloc() functions return a pointer to the
  * allocated memory that is suitably aligned for any kind of variable. On
  * error, these functions return NULL. NULL may also be returned by a
  * successful call to malloc() with a size of zero, or by a successful call to
  * calloc() with nmemb or size equal to zero.
  *
  * The malloc() function allocates size bytes and returns a pointer to the
  * allocated memory. The memory is not initialized. If size is 0, then
  * malloc() returns either NULL, or a unique pointer value that can later be
  * successfully passed to free().
  *
  * @see malloc, free, calloc, realloc
  * @see brk, mmap, alloca, malloc_get_state, malloc_info, malloc_trim,
  * malloc_usable_size, mallopt, mcheck, mtrace, posix_memalign
  * @note By default, the kernel follows an optimistic memory allocation
  * strategy. This means that when malloc() returns non-NULL there is no
  * guarantee that the memory really is available. In case it turns out that
  * the system is out of memory, one or more processes will be killed.
  */
void *_malloc(size_t length)
{
  void *ptr;
  struct SMK_HeapArea *area;
  ll_for_each (&gHeapArea, area, struct SMK_HeapArea, node_) {
    ptr = malloc_r(area, length);

    if (ptr)
      return ptr;
  }

  return NULL;
}


/* ----------------------------------------------------------------------- */
/** @brief Free dynamic memory
  * @return The free() function returns no value.
  *
  * The free() function frees the memory space pointed to by ptr, which must
  * have been returned by a previous call to malloc(), calloc() or realloc().
  * Otherwise, or if free(ptr) has already been called before, undefined
  * behavior occurs. If ptr is NULL, no operation is performed.
  *
  * @see malloc, free, calloc, realloc
  * @see brk, mmap, alloca, malloc_get_state, malloc_info, malloc_trim,
  * malloc_usable_size, mallopt, mcheck, mtrace, posix_memalign
  */
void _free(void *ptr)
{
  struct SMK_HeapArea *area;
  ll_for_each (&gHeapArea, area, struct SMK_HeapArea, node_) {
    if (area->begin_ >= (size_t)ptr && area->end_ < (size_t)ptr) {
      free_r (area, ptr);
      return;
    }
  }
}


/* ----------------------------------------------------------------------- */
/** @brief Definition of a new heap arena */
void alloc_arena(size_t base, size_t length,
    int(*freed)(size_t base,size_t length))
{
  struct SMK_HeapArea *area = &gArea;

  if (gHeapArea.count_ != 0)
    area = _malloc(sizeof(struct SMK_HeapArea));

  area->flags |= ALLOC_PARANOID;
  area->start = ( struct SMK_HeapChunk *) ALIGN ( (uintptr_t)base, ALLOC_MIN_CHUNK );
  area->begin_ = (size_t)base;
  area->end_ = (size_t)base + length;
  area->free_list = NULL;
  area->available = (size_t)area->start - (size_t)base;
  area->available = length - area->available;
  area->available = area->available & ~(ALLOC_MIN_CHUNK - 1);
  area->max = (size_t)area->start + area->available;
  area->start->prev_size = 0;
  area->start->chunk_size = area->available;
  area->flags |= ALLOC_CHECK;
  area->freed_ = freed;
  area->lock = 0;
  alloc_add_to_free (area, area->start);

  ll_push_back(&gHeapArea, & area->node_);
}

#ifdef __USE_SCALL

void* _mmap(void* address, size_t length, int fd, int offset, int flags);

void __axc_heap(void)
{
  long size = _Mb_;
  // long heap_base = brk(0);
  // long heap_lim = brk(heap_base + size);
  // assert (size = heap_lim - heap_base);
  // int right = VMA_HEAP | VMA_WRITE | VMA_READ | VMA_GROWSUP;
  long heap_base = _mmap(NULL, size, -1, 0, 0x1276);

  memset(&gHeapArea, 0, sizeof(gHeapArea));
  memset(&gArea, 0, sizeof(gArea));
  alloc_arena(heap_base, size, NULL);
}

#endif

/* ----------------------------------------------------------------------- */
/* ----------------------------------------------------------------------- */
