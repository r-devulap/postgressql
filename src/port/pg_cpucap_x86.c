/*-------------------------------------------------------------------------
 *
 * pg_cpucap_x86.c
 *	  Check if the CPU we're running on supports SSE4.2.
 *
 * Portions Copyright (c) 1996-2025, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/port/pg_cpucap_x86.c
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

#include "port/pg_cpucap.h"

static bool
pg_sse42_available(void)
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

/*
 * Check if hardware instructions for CRC computation are available.
 */
void
pg_cpucap_crc32c(void)
{
	if (pg_sse42_available())
		pg_cpucap |= PGCPUCAP_CRC32C;
}
