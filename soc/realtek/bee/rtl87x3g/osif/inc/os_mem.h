/*
 * Copyright (c) 2017, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _OS_MEM_H_
#define _OS_MEM_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    Memory  Memory Management
 *
 * \brief   Allocate, free, and peek memory functions.
 * \details The Memory Management function group allows to allocate, free, and peek heap
 *          memory in the system.\n
 *
 */

typedef enum
{
    OS_MEM_TYPE_DATA = 0x00,
    OS_MEM_TYPE_BUFFER = 0x02,
    OS_MEM_TYPE_SHARING = 0x06,
    OS_MEM_TYPE_NUM = 0x07,
} T_OS_MEM_TYPE;

void *os_mem_alloc_intern(T_OS_MEM_TYPE ram_type, size_t size);

void *os_mem_zalloc_intern(T_OS_MEM_TYPE ram_type, size_t size);

void *os_mem_aligned_alloc_intern(T_OS_MEM_TYPE ram_type, size_t size, uint8_t alignment);

/**
 * os_mem.h
 *
 * \brief    Allocate a memory block with required size.
 *
 * \param[in]   mem_type            mem type for allocation.
 * \arg \c      OS_MEM_TYPE_DATA    DATA mem type.
 * \arg \c      OS_MEM_TYPE_BUFFER  BUFFER mem type.
 * \arg \c      OS_MEM_TYPE_SHARING SHARING mem type.
 *
 * \param[in]   size     Required memory size.
 *
 * \return     The address of the allocated memory block. If the address is NULL, the
 *             memory allocation failed.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_alloc(OS_MEM_TYPE_DATA, mem_size);
 *     if (p_mem != NULL)
 *     {
 *         // Memory allocation succeeded, and free it.
 *         os_mem_free(p_mem);
 *     }
 *     else
 *     {
 *         // Memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 *
 */
#define os_mem_alloc(ram_type, size)    \
    os_mem_alloc_intern(ram_type, size)

/**
 * os_mem.h
 *
 * \brief    Allocate and clear a memory block with required size.
 *
 * \param[in]   mem_type            mem type for allocation.
 * \arg \c      OS_MEM_TYPE_DATA    DATA mem type.
 * \arg \c      OS_MEM_TYPE_BUFFER  BUFFER mem type.
 * \arg \c      OS_MEM_TYPE_SHARING SHARING mem type.
 * \param[in]   size     Required memory size.
 *
 * \return     The address of the allocated memory block. If the address is NULL, the
 *             memory allocation failed.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_zalloc(OS_MEM_TYPE_DATA, mem_size);
 *     if (p_mem != NULL)
 *     {
 *         // Memory allocation succeeded, and free it.
 *         os_mem_free(p_mem);
 *     }
 *     else
 *     {
 *         // Memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 *
 */
#define os_mem_zalloc(ram_type, size)   \
    os_mem_zalloc_intern(ram_type, size)

/**
 * os_mem.h
 *
 * \brief    Allocate an aligned memory block with required size.
 *
 * \param[in]   mem_type            mem type for allocation.
 * \arg \c      OS_MEM_TYPE_DATA    DATA mem type.
 * \arg \c      OS_MEM_TYPE_BUFFER  BUFFER mem type.
 * \arg \c      OS_MEM_TYPE_SHARING SHARING mem type.
 *
 * \param[in]   size        Required memory size.
 *
 * \param[in]   alignment   memory alignment in 2^N bytes. If alignment is 0, use
 *                          system default memory alignment. The aligned memory block
 *                          must use os_mem_aligned_free() API function to free.
 *
 * \return     The address of the allocated memory block. If the address is NULL, the
 *             memory allocation failed.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     uint8_t mem_alignment = 16;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_aligned_alloc(OS_MEM_TYPE_DATA, mem_size, mem_alignment);
 *     if (p_mem != NULL)
 *     {
 *         // Aligned memory allocation succeeded, and free it.
 *         os_mem_aligned_free(p_mem);
 *     }
 *     else
 *     {
 *         // Aligned memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 *
 */
#define os_mem_aligned_alloc(ram_type, size, alignment) \
    os_mem_aligned_alloc_intern(ram_type, size, alignment)

/**
 * os_mem.h
 *
 * \brief    Free a memory block that had been allocated.
 *
 * \param[in]   p_block     The address of memory block being freed.
 *
 * \return     None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_alloc(OS_MEM_TYPE_DATA, mem_size);
 *     if (p_mem != NULL)
 *     {
 *         // Memory allocation succeeded, and free it.
 *         os_mem_free(p_mem);
 *     }
 *     else
 *     {
 *         // Memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 *
 */
void os_mem_free(void *p_block);

/**
 * os_mem.h
 *
 * \brief    Free an aligned memory block that had been allocated.
 *
 * \param[in]   p_block     The address of memory block being freed.
 *
 * \return     None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t mem_size = 0x1000;
 *     uint8_t mem_alignment = 16;
 *     void *p_mem = NULL;
 *
 *     p_mem = os_mem_aligned_alloc(OS_MEM_TYPE_DATA, mem_size, mem_alignment);
 *     if (p_mem != NULL)
 *     {
 *         // Aligned memory allocation succeeded, and free it.
 *         os_mem_aligned_free(p_mem);
 *     }
 *     else
 *     {
 *         // Aligned memory allocation failed.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 *
 */
void os_mem_aligned_free(void *p_block);


/**
 * os_mem.h
 *
 * \brief    Peek the unused memory size of the specified mem type.
 *
 * \param[in]   mem_type            mem type for allocation.
 * \arg \c      OS_MEM_TYPE_DATA    DATA mem type.
 * \arg \c      OS_MEM_TYPE_BUFFER  BUFFER mem type.
 * \arg \c      OS_MEM_TYPE_SHARING SHARING mem type.
 *
 * \return     The unused memory size in btyes.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     size_t unused_data;
 *     size_t unused_buffer;
 *
 *     // Peek unused DATA memory size.
 *     unused_size = os_mem_peek(OS_MEM_TYPE_DATA);
 *
 *     // Peek unused BUFFER memory size.
 *     unused_size = os_mem_peek(OS_MEM_TYPE_BUFFER);
 *
 *     return 0;
 * }
 * \endcode
 *
 *
 */
size_t os_mem_peek(T_OS_MEM_TYPE ram_type);

void os_mem_peek_printf(void);
#ifdef __cplusplus
}
#endif

#endif /* _OS_MEM_H_ */
