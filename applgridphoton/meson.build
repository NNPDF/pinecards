project(
    'pineappl',
    'cpp',
    default_options : [ 'cpp_std=c++14', 'debug=false', 'optimization=2', 'warning_level=3' ],
    license : 'GPL3',
    meson_version : '>=0.51.0',
    version : '1.4.70'
)

incdir = include_directories('.')

headers = [
    'appl_grid/SparseMatrix3d.h',
    'appl_grid/appl_grid.h',
    'appl_grid/appl_igrid.h',
    'appl_grid/appl_pdf.h',
    'appl_grid/axis.h',
    'appl_grid/combination.h',
    'appl_grid/histo1d.hpp',
    'appl_grid/lumi_pdf.h',
    'appl_grid/tsparse3d.h',
    'appl_grid/tsparse_base.h',
]

install_headers(headers, subdir : 'appl_grid')

subdir('bin')
subdir('include')
subdir('src')
subdir('tests')

if get_option('doc')
    subdir('doc')
endif

pkg = import('pkgconfig')
pkg.generate(
    description : 'PineAPPL is not an extension of APPLgrid',
    name : meson.project_name(),
    version : meson.project_version(),
    libraries : lib
)
