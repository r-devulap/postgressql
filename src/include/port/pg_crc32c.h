/*-------------------------------------------------------------------------
 *
 * pg_crc32c.h
 *	  Routines for computing CRC-32C checksums.
 *
 * The speed of CRC-32C calculation has a big impact on performance, so we
 * jump through some hoops to get the best implementation for each
 * platform. Some CPU architectures have special instructions for speeding
 * up CRC calculations (e.g. Intel SSE 4.2), on other platforms we use the
 * Slicing-by-8 algorithm which uses lookup tables.
 *
 * The public interface consists of four macros:
 *
 * INIT_CRC32C(crc)
 *		Initialize a CRC accumulator
 *
 * COMP_CRC32C(crc, data, len)
 *		Accumulate some (more) bytes into a CRC
 *
 * FIN_CRC32C(crc)
 *		Finish a CRC calculation
 *
 * EQ_CRC32C(c1, c2)
 *		Check for equality of two CRCs.
 *
 * Portions Copyright (c) 1996-2025, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/port/pg_crc32c.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_CRC32C_H
#define PG_CRC32C_H

#include "port/pg_bswap.h"
#include "port/pg_cpucap.h"

typedef uint32 pg_crc32c;

/* The INIT and EQ macros are the same for all implementations. */
#define INIT_CRC32C(crc) ((crc) = 0xFFFFFFFF)
#define EQ_CRC32C(c1, c2) ((c1) == (c2))

#if defined(USE_SSE42_CRC32C) || defined(USE_SSE42_CRC32C_WITH_RUNTIME_CHECK)
/* Use Intel SSE4.2 instructions. */
#define COMP_CRC32C(crc, data, len) \
	((crc) = pg_comp_crc32c_dispatch((crc), (data), (len)))
#define COMP_CRC32C_HW(crc, data, len) \
	((crc) = pg_comp_crc32c_sse42((crc), (data), (len)))
#define FIN_CRC32C(crc) ((crc) ^= 0xFFFFFFFF)

#if defined(USE_SSE42_CRC32C)
#define HAVE_CRC_COMPTIME
#else
#define HAVE_CRC_RUNTIME
extern pg_crc32c pg_comp_crc32c_sb8(pg_crc32c crc, const void *data, size_t len);
#endif

extern bool pg_crc32c_sse42_available(void);
extern pg_crc32c pg_comp_crc32c_sse42(pg_crc32c crc, const void *data, size_t len);

#elif defined(USE_ARMV8_CRC32C) || defined(USE_ARMV8_CRC32C_WITH_RUNTIME_CHECK)
/* Use ARMv8 CRC Extension instructions. */

#define COMP_CRC32C(crc, data, len)							\
	((crc) = pg_comp_crc32c_dispatch((crc), (data), (len)))
#define COMP_CRC32C_HW(crc, data, len)						\
	((crc) = pg_comp_crc32c_armv8((crc), (data), (len)))
#define FIN_CRC32C(crc) ((crc) ^= 0xFFFFFFFF)

#if defined(USE_ARMV8_CRC32C)
#define HAVE_CRC_COMPTIME
#else
#define HAVE_CRC_RUNTIME
extern pg_crc32c pg_comp_crc32c_sb8(pg_crc32c crc, const void *data, size_t len);
#endif

extern bool pg_crc32c_armv8_available(void);
extern pg_crc32c pg_comp_crc32c_armv8(pg_crc32c crc, const void *data, size_t len);

#elif defined(USE_LOONGARCH_CRC32C)
/* Use LoongArch CRCC instructions. */

#define COMP_CRC32C(crc, data, len)							\
	((crc) = pg_comp_crc32c_dispatch((crc), (data), (len)))
#define COMP_CRC32C_HW(crc, data, len)						\
	((crc) = pg_comp_crc32c_loongarch((crc), (data), (len)))
#define FIN_CRC32C(crc) ((crc) ^= 0xFFFFFFFF)

#define HAVE_CRC_COMPTIME
extern pg_crc32c pg_comp_crc32c_loongarch(pg_crc32c crc, const void *data, size_t len);

#else
/*
 * Use slicing-by-8 algorithm.
 *
 * On big-endian systems, the intermediate value is kept in reverse byte
 * order, to avoid byte-swapping during the calculation. FIN_CRC32C reverses
 * the bytes to the final order.
 */
#define COMP_CRC32C(crc, data, len) \
	((crc) = pg_comp_crc32c_sb8((crc), (data), (len)))
#ifdef WORDS_BIGENDIAN
#define FIN_CRC32C(crc) ((crc) = pg_bswap32(crc) ^ 0xFFFFFFFF)
#else
#define FIN_CRC32C(crc) ((crc) ^= 0xFFFFFFFF)
#endif

extern pg_crc32c pg_comp_crc32c_sb8(pg_crc32c crc, const void *data, size_t len);

#endif							/* end of CPU-specfic symbols */

#if defined(HAVE_CRC_COMPTIME) || defined(HAVE_CRC_RUNTIME)
/*
 * Check if the CPU we're running on supports special
 * instructions for CRC-32C computation. Otherwise, fall
 * back to the pure software implementation (slicing-by-8).
 */
static inline pg_crc32c
pg_comp_crc32c_dispatch(pg_crc32c crc, const void *data, size_t len)
{
	/*
	 * If this is firing in a frontend program, first look if you forgot a
	 * call to pg_cpucap_initialize() in main(). See for example
	 * src/bin/pg_controldata/pg_controldata.c.
	 */
	// WIP: how to best intialize in frontend?
#ifndef FRONTEND
	Assert(pg_cpucap & PGCPUCAP_INIT);
#endif

#if defined(HAVE_CRC_COMPTIME)
	return COMP_CRC32C_HW(crc, data, len);
#else
	if (pg_cpucap & PGCPUCAP_CRC32C)
		return COMP_CRC32C_HW(crc, data, len);
	else
		return pg_comp_crc32c_sb8(crc, data, len);
#endif
}
#endif							/* HAVE_CRC_COMPTIME || HAVE_CRC_RUNTIME */

#endif							/* PG_CRC32C_H */
