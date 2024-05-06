CREATE FUNCTION drive_crc32c  (count int, num int) RETURNS bigint AS 'MODULE_PATHNAME' LANGUAGE C;
