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


/* starts uninitialized so we can detect errors of omission */
uint32		pg_cpucap = 0;

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
	pg_cpucap_clmul();
}
