project('libSAOCPPYield', 'cpp',
  version : '1.0.0',
  license : 'GPL-3.0-only',
  default_options : [
    'warning_level=1',
    'werror=true'
  ]
)

src = [
  'src/tree.cpp'
]

inc = [
  include_directories('include')
]

deps = []

lib = static_library('SAOCPPYield', src,
  include_directories : inc,
  dependencies : deps
)

dep = declare_dependency(
  include_directories : inc,
  link_with : [
    lib
  ]
)

if get_option('debug')
  subdir('test')
endif
