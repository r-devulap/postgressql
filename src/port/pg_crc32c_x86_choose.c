/*-------------------------------------------------------------------------
 *
 * pg_crc32c_x86_choose.c
 *	  Choose between Intel AVX-512, SSE 4.2 and software CRC-32C implementation.
 *
 * On first call, checks if the CPU we're running on supports Intel AVX-512. If
 * it does, use the special SSE instructions for CRC-32C computation.
 * Otherwise, fall back to the pure software implementation (slicing-by-8).
 *
 * Portions Copyright (c) 1996-2024, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/port/pg_crc32c_x86_choose.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"

#include "port/pg_crc32c.h"
#include "port/pg_hw_feat_check.h"

/*
 * This gets called on the first call. It replaces the function pointer
 * so that subsequent calls are routed directly to the chosen implementation.
 */
static pg_crc32c
pg_comp_crc32c_choose(pg_crc32c crc, const void *data, size_t len)
{
        pg_comp_crc32c = pg_comp_crc32c_sb8;
#ifdef USE_SSE42_CRC32C
        pg_comp_crc32c = pg_comp_crc32c_sse42;
#elif USE_SSE42_CRC32C_WITH_RUNTIME_CHECK
        if (pg_crc32c_sse42_available())
		pg_comp_crc32c = pg_comp_crc32c_sse42;
#endif
#ifdef USE_AVX512_CRC32C_WITH_RUNTIME_CHECK
	if (pg_crc32c_avx512_available())
		pg_comp_crc32c = pg_comp_crc32c_avx512;
#endif
	return pg_comp_crc32c(crc, data, len);
}

pg_crc32c (*pg_comp_crc32c) (pg_crc32c crc, const void *data, size_t len) = pg_comp_crc32c_choose;
