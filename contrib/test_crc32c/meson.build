# Copyright (c) 2022-2024, PostgreSQL Global Development Group

test_crc32c_sources = files(
  'test_crc32c.c',
)

if host_system == 'windows'
  test_crc32c_sources += rc_lib_gen.process(win32ver_rc, extra_args: [
    '--NAME', 'test_crc32c',
    '--FILEDESC', 'test_crc32c - test code for crc32c library',])
endif

test_crc32c = shared_module('test_crc32c',
  test_crc32c_sources,
  kwargs: contrib_mod_args,
)
contrib_targets += test_crc32c

install_data(
  'test_crc32c.control',
  'test_crc32c--1.0.sql',
  kwargs: contrib_data_args,
)

tests += {
  'name': 'test_crc32c',
  'sd': meson.current_source_dir(),
  'bd': meson.current_build_dir(),
  'regress': {
    'sql': [
      'test_crc32c',
    ],
  },
}
