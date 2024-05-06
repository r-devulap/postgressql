/* select drive_crc32c(1000000, 1024); */

#include "postgres.h"
#include "fmgr.h"
#include "port/pg_crc32c.h"
#include "common/pg_prng.h"

PG_MODULE_MAGIC;

/*
 * drive_crc32c(count: int, num: int) returns bigint
 *
 * count is the nuimber of loops to perform
 *
 * num is the number byte in the buffer to calculate
 * crc32c over.
 */
PG_FUNCTION_INFO_V1(drive_crc32c);
Datum
drive_crc32c(PG_FUNCTION_ARGS)
{
	int64			count	= PG_GETARG_INT64(0);
	int64			num		= PG_GETARG_INT64(1);
	char*		data	= malloc((size_t)num);
	pg_crc32c crc;
	pg_prng_state state;
	uint64 seed = 42;
	pg_prng_seed(&state, seed);
	/* set random data */
	for (uint64 i = 0; i < num; i++)
	{
		data[i] = pg_prng_uint32(&state) % 255;
	}

	INIT_CRC32C(crc);

	while(count--)
	{
		INIT_CRC32C(crc);
		COMP_CRC32C(crc, data, num);
		FIN_CRC32C(crc);
	}

	free((void *)data);

	PG_RETURN_INT64((int64_t)crc);
}
