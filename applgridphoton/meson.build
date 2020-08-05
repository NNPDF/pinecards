project(
    'pineappl_capi',
    'cpp',
    default_options : [ 'cpp_std=c++14', 'debug=false', 'optimization=2', 'warning_level=3' ],
    license : 'GPL3',
    meson_version : '>=0.51.0',
    version : '1.4.70'
)

papplgrid_incdir = include_directories('.')

subdir('bin')
subdir('include')
subdir('src')
subdir('tests')

if get_option('doc')
    subdir('doc')
endif
