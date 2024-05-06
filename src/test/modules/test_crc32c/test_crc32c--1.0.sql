CREATE FUNCTION drive_crc32c  (count int, num int) RETURNS bigint AS 'test_crc32c.so' LANGUAGE C;
