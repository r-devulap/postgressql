/*-------------------------------------------------------------------------
 *
 * pg_cpucap.h
 *	  Runtime detection of CPU capabilities.
 *
 * Portions Copyright (c) 1996-2025, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 *	  src/include/port/pg_cpucap.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_CPUCAP_H
#define PG_CPUCAP_H

#define PGCPUCAP_INIT           (1 << 0)
#define PGCPUCAP_POPCNT         (1 << 1)
#define PGCPUCAP_VPOPCNT        (1 << 2)
#define PGCPUCAP_CRC32C         (1 << 3)

extern PGDLLIMPORT uint32 pg_cpucap;
extern void pg_cpucap_initialize(void);

#endif							/* PG_CPUCAP_H */
