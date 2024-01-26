project(
  'linvoke',
  'c',
  version: '1.0.0',
  license: 'MIT',
  default_options: [
    'default_library=static',
    'warning_level=3'
  ],
)

install_headers('include/linvoke.h')
linvoke_include_directories = include_directories('include')

# Library target
linvoke_lib = library(
  'linvoke',
  'source/linvoke.c',
  include_directories: linvoke_include_directories,
  install: true,
)

# Declare a dependency for the library
linvoke_dep = declare_dependency(
  include_directories: linvoke_include_directories, 
  link_with: linvoke_lib,
)

# Generate pkg-config file for the library
pkg = import('pkgconfig')
pkg.generate(linvoke_lib)

# Build the hello world example 
if get_option('compile_examples')
  linvoke_example_simple_event_executable = executable(
    'linvoke-simple-event',
    'examples/simple_event.c',
    dependencies: [linvoke_dep],
  )
  linvoke_example_simple_event_with_data_executable = executable(
    'linvoke-simple-event-with-data',
    'examples/simple_event_with_data.c',
    dependencies: [linvoke_dep],
  )
  linvoke_example_multi_slot_event_executable = executable(
    'linvoke-multi-slot-event',
    'examples/multi_slot_event.c',
    dependencies: [linvoke_dep],
  )
endif

# Testing using CMocka
cmocka_dep = dependency('cmocka')

test('linvoke_test',
  executable(
    'linvoke-test',
    'test/test.c',
    dependencies: [linvoke_dep, cmocka_dep],
  )
)
