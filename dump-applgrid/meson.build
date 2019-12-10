project('plot-applgrid', 'cpp', default_options : [ 'cpp_std=c++14' ])

applgrid_dep = meson.get_compiler('cpp').find_library('APPLgrid',
    has_headers : [ 'appl_grid/appl_grid.h' ])
root_dep = dependency('ROOT', method : 'cmake', modules : [ 'ROOT::Core', 'ROOT::Hist' ])

executable('dump-applgrid', 'main.cpp', dependencies : [ applgrid_dep, root_dep ])
