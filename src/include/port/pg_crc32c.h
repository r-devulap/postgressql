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

#include "pg_cpu.h"
#include "port/pg_bswap.h"

typedef uint32 pg_crc32c;

/* The INIT and EQ macros are the same for all implementations. */
#define INIT_CRC32C(crc) ((crc) = 0xFFFFFFFF)
#define EQ_CRC32C(c1, c2) ((c1) == (c2))
#define FIN_CRC32C(crc) ((crc) ^= 0xFFFFFFFF)

/* x86 / x86_64 */
#if defined(PG_CPU_X86) || defined(PG_CPU_x86_64)
extern pg_crc32c pg_comp_crc32c_sb8(pg_crc32c crc, const void *data, size_t len);
extern pg_crc32c pg_comp_crc32c_sse42(pg_crc32c crc, const void *data, size_t len);
extern pg_crc32c pg_comp_crc32c_pclmul(pg_crc32c crc, const void *data, size_t len);
extern pg_crc32c (*pg_comp_crc32c) (pg_crc32c crc, const void *data, size_t len);
#define COMP_CRC32C(crc, data, len) \
	((crc) = pg_comp_crc32c((crc), (data), (len)))

/* ARMV8 */
#elif defined(USE_ARMV8_CRC32C)
extern pg_crc32c pg_comp_crc32c_armv8(pg_crc32c crc, const void *data, size_t len);
#define COMP_CRC32C(crc, data, len)							\
	((crc) = pg_comp_crc32c_armv8((crc), (data), (len)))

/* ARMV8 with runtime check */
#elif defined(USE_ARMV8_CRC32C_WITH_RUNTIME_CHECK)
extern pg_crc32c pg_comp_crc32c_sb8(pg_crc32c crc, const void *data, size_t len);
extern pg_crc32c pg_comp_crc32c_armv8(pg_crc32c crc, const void *data, size_t len);
extern pg_crc32c (*pg_comp_crc32c) (pg_crc32c crc, const void *data, size_t len);
#define COMP_CRC32C(crc, data, len) \
	((crc) = pg_comp_crc32c((crc), (data), (len)))

/* LoongArch */
#elif defined(USE_LOONGARCH_CRC32C)
extern pg_crc32c pg_comp_crc32c_loongarch(pg_crc32c crc, const void *data, size_t len);
#define COMP_CRC32C(crc, data, len)							\
	((crc) = pg_comp_crc32c_loongarch((crc), (data), (len)))

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
#undef FIN_CRC32C
#define FIN_CRC32C(crc) ((crc) = pg_bswap32(crc) ^ 0xFFFFFFFF)
#endif

extern pg_crc32c pg_comp_crc32c_sb8(pg_crc32c crc, const void *data, size_t len);
#endif

#endif							/* PG_CRC32C_H */
