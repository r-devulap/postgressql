/*-------------------------------------------------------------------------
 *
 * pg_cpucap.c
 *	  Runtime detection of CPU capabilities.
 *
 * Portions Copyright (c) 1996-2025, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 *	  src/port/pg_cpucap.c
 *
 *-------------------------------------------------------------------------
 */
#include "c.h"

#include "port/pg_cpucap.h"
#include "port/pg_crc32c.h"


/* starts uninitialized so we can detect errors of omission */
uint32		pg_cpucap = 0;

/*
 * Check if hardware instructions for CRC computation are available.
 */
static void
pg_cpucap_crc32c(void)
{
	/* WIP: It seems like we should use CPU arch symbols instead */
#if defined(USE_SSE42_CRC32C) || defined(USE_SSE42_CRC32C_WITH_RUNTIME_CHECK)
	if (pg_crc32c_sse42_available())
		pg_cpucap |= PGCPUCAP_CRC32C;

#elif defined(USE_ARMV8_CRC32C) || defined(USE_ARMV8_CRC32C_WITH_RUNTIME_CHECK)
	if (pg_crc32c_armv8_available())
		pg_cpucap |= PGCPUCAP_CRC32C;
#endif
}

/*
 * This needs to be called in main() for every
 * program that calls a function that dispatches
 * according to CPU features.
 */
void
pg_cpucap_initialize(void)
{
	pg_cpucap = PGCPUCAP_INIT;

	pg_cpucap_crc32c();
}
