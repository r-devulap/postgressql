/*-------------------------------------------------------------------------
 *
 * pg_crc32c_sse42_choose.c
 *	  Choose between Intel SSE 4.2 and software CRC-32C implementation.
 *
 * On first call, checks if the CPU we're running on supports Intel SSE
 * 4.2. If it does, use the special SSE instructions for CRC-32C
 * computation. Otherwise, fall back to the pure software implementation
 * (slicing-by-8).
 *
 * Portions Copyright (c) 1996-2025, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/port/pg_crc32c_sse42_choose.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"

#ifdef HAVE__GET_CPUID
#include <cpuid.h>
#endif

#ifdef HAVE__CPUID
#include <intrin.h>
#endif

#include "port/pg_crc32c.h"

#if !defined(USE_PCLMUL_CRC32C)
#if !defined(USE_SSE42_CRC32C)
static bool
pg_crc32c_sse42_available(void)
{
	unsigned int exx[4] = {0, 0, 0, 0};

#if defined(HAVE__GET_CPUID)
	__get_cpuid(1, &exx[0], &exx[1], &exx[2], &exx[3]);
#elif defined(HAVE__CPUID)
	__cpuid(exx, 1);
#else
#error cpuid instruction not available
#endif

    return (exx[2] & (1 << 20)) != 0;	/* SSE 4.2 */
}
#endif // !USE_SSE42_CRC32C

static bool
pg_crc32c_pclmul_available(void)
{
	unsigned int exx[4] = {0, 0, 0, 0};

#if defined(HAVE__GET_CPUID)
	__get_cpuid(1, &exx[0], &exx[1], &exx[2], &exx[3]);
#elif defined(HAVE__CPUID)
	__cpuid(exx, 1);
#else
#error cpuid instruction not available
#endif

	return (exx[2] & (1 << 1)) != 0;	/* PCLMULQDQ */
}

#endif // !USE_PCLMUL_CRC32C

/*
 * This gets called on the first call. It replaces the function pointer
 * so that subsequent calls are routed directly to the chosen implementation.
 */
static pg_crc32c
pg_comp_crc32c_choose(pg_crc32c crc, const void *data, size_t len)
{
#ifdef USE_PCLMUL_CRC32C
        pg_comp_crc32c = pg_comp_crc32c_pclmul; /* SSE42 and PCLUMUL assumed*/
#elif USE_SSE42_CRC32C
        pg_comp_crc32c = pg_comp_crc32c_sse42; /* SSE42 assumed*/
        if (pg_crc32c_pclmul_available())
            pg_comp_crc32c = pg_comp_crc32c_pclmul;
#else
        pg_comp_crc32c = pg_comp_crc32c_sb8;
#ifdef USE_SSE42_CRC32C_WITH_RUNTIME_CHECK
        if (pg_crc32c_sse42_available())
            pg_comp_crc32c = pg_comp_crc32c_sse42;
#endif
#ifdef USE_PCLMUL_CRC32C_WITH_RUNTIME_CHECK
        if (pg_crc32c_pclmul_available())
            pg_comp_crc32c = pg_comp_crc32c_pclmul;
#endif
#endif
        return pg_comp_crc32c(crc, data, len);
}

PGDLLIMPORT pg_crc32c	(*pg_comp_crc32c) (pg_crc32c crc, const void *data, size_t len) = pg_comp_crc32c_choose;
