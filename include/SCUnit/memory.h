#ifndef SCUNIT_MEMORY_H
#define SCUNIT_MEMORY_H

#include <stdlib.h>

/**
 * @brief Allocates a block of uninitialized memory.
 *
 * @attention If `size` is zero, the behavior is implementation-defined.
 *
 * @param[in] size Size of the block to allocate (in bytes).
 * @return A pointer to an uninitialized block of memory or a `nullptr` if the allocation failed due
 * to an out-of-memory condition.
 */
#define SCUNIT_MALLOC(size) malloc(size)

/**
 * @brief Allocates a block of zero-initialized memory.
 *
 * @attention If `size` is zero, the behavior is implementation-defined.
 *
 * @param[in] count Number of elements in the block.
 * @param[in] size  Size of each element (in bytes).
 * @return A pointer to a zero-initialized block of memory or a `nullptr` if the allocation failed
 * due to an out-of-memory condition.
 */
#define SCUNIT_CALLOC(count, size) calloc(count, size)

/**
 * @brief Reallocates a previously allocated block of memory.
 *
 * @warning If `size` is zero, the behavior is undefined.
 *
 * If the reallocation fails due to an out-of-memory condition, the pointer to the original block of
 * memory remains valid and must be passed to `SCUNIT_FREE()` to avoid a memory leak. Therefore,
 * do not assign the return value of `SCUNIT_REALLOC()` directly to the original pointer, as this
 * could result in losing the reference to the original memory block.
 *
 * If the reallocation succeeds, the pointer to the original block of memory is invalidated and must
 * not be used anymore (and especially not be passed to `SCUNIT_FREE()`). The content of any new
 * memory locations (in case the block was expanded) is undefined.
 *
 * @param[in] pointer Pointer to the block of memory to reallocate. It is allowed to be `nullptr`,
 *                    in which case it behaves like calling `SCUNIT_MALLOC()` with the given size.
 * @param[in] size    Size to reallocate the block to (in bytes).
 * @return A pointer to the reallocated block of memory or a `nullptr` if the allocation failed due
 * to an out-of-memory condition.
 */
#define SCUNIT_REALLOC(pointer, size) realloc(pointer, size)

/**
 * @brief Deallocates a block of memory previously allocated by `SCUNIT_MALLOC()`, `SCUNIT_CALLOC()`
 * or `SCUNIT_REALLOC()`.
 *
 * @note For convenience, `pointer` is allowed to be `nullptr`.
 *
 * @warning The behavior is undefined if the block of memory pointed to by `pointer` was not
 * previously allocated by `SCUNIT_MALLOC()`, `SCUNIT_CALLOC()` or `SCUNIT_REALLOC()` or if it has
 * already been deallocated.
 *
 * @param[in] pointer Pointer to the block of memory to deallocate.
 */
#define SCUNIT_FREE(pointer) free(pointer)

#endif