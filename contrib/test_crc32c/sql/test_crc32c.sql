CREATE EXTENSION test_crc32c;

select drive_crc32c(1, i) from generate_series(100, 300, 4) i;
