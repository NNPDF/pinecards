project('applcheck', 'cpp', default_options : [ 'warning_level=3' ], version : '0.1.0')

root_dep = dependency('ROOT', modules : [ 'ROOT::Core', 'ROOT::Hist' ])
lhapdf_dep = dependency('lhapdf')
appl_grid_dep = meson.get_compiler('cpp').find_library('APPLgrid',
    has_headers : 'appl_grid/appl_grid.h')

executable('applcheck', 'applcheck.cc', dependencies : [ appl_grid_dep, lhapdf_dep, root_dep ])
