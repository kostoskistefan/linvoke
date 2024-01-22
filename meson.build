project(
  'linvoke',
  'c',
  version: '1.0.0',
  license: 'MIT',
  default_options: [
    'default_library=static'
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
  linvoke_hello_world_example_executable = executable(
    'linvoke-hello-world',
    'examples/hello_world.c',
    dependencies: [linvoke_dep],
  )
endif

# Testing using CMocka
cmocka_dep = dependency('cmocka')

test('linvoke_test',
  executable(
    'linvoke_test',
    'test/test.c',
    dependencies: [linvoke_dep, cmocka_dep],
  )
)
