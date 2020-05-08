srcs1 = [
    'SparseMatrix3d.cxx',
    'TFileString.cxx',
    'TFileVector.cxx',
    'appl_grid.cxx',
    'appl_igrid.cxx',
    'appl_pdf.cxx',
    'appl_timer.cxx',
    'combination.cxx',
    'histo1d.cpp',
    'hoppet_init.cxx',
    'lumi_pdf.cxx',
    'sparse_matrix3d_root_interface.cpp',
    'tsparse3d.cxx',
    'tsparse_base.cxx',
]

root_dep = dependency('ROOT', method : 'cmake', modules : [ 'ROOT::Core', 'ROOT::Hist'
#, 'ROOT::rootcling'
])

# TODO: instead of finding `rootcling` we should probably extract the CMake variable, but I'm not
# sure how to do that
root_int = find_program('rootcling')

root_gen = generator(
    root_int,
    arguments : [ '-f', '@OUTPUT0@', '@CURRENT_SOURCE_DIR@/@BASENAME@.h'],
    output : [ '@BASENAME@Dict.cxx', '@BASENAME@Dict_rdict.pcm' ]
)

srcs2 = [
    root_gen.process('TFileString.cxx'),
    root_gen.process('TFileVector.cxx'),
]

config = configuration_data()
config.set_quoted('PACKAGE_STRING', meson.project_name())
# TODO: add proper bugreport email address
config.set_quoted('PACKAGE_BUGREPORT', 'this-project-has-no@bugs.it')
config.set_quoted('PACKAGE_VERSION', meson.project_version())
config.set_quoted('DATADIR', get_option('prefix') / get_option('datadir'))

conf_header = configure_file(configuration : config, output : 'amconfig.h')

lib = library(
    'APPLgrid',
    srcs1, srcs2,
    include_directories : incdir,
    dependencies : root_dep,
    install : true
)

dep = declare_dependency(include_directories : incdir, link_with : lib, dependencies : root_dep)

lhapdf_dep = dependency('lhapdf')

executable('merge_bins', 'merge_bins.cpp', dependencies : dep, install : true)
executable('pdf_info', 'pdf_info.cpp', dependencies : dep, install : true)
executable('applcheck', 'applcheck.cpp', dependencies : [ dep, lhapdf_dep ], install : true)
executable('dump-applgrid', 'dump-applgrid.cpp', dependencies : dep, install : true)

lib2 = library(
    'pineappl_capi',
    'pineappl_capi.cpp',
    include_directories : incdir2,
    dependencies : dep,
    install : true
)

dep2 = declare_dependency(include_directories : incdir2, link_with : lib2, dependencies : dep)

executable('applgrid-combine', 'combine.cxx', dependencies : dep2, install : true)

import('pkgconfig').generate(
    description : 'PineAPPL is not an extension of APPLgrid',
    name : meson.project_name(),
    version : meson.project_version(),
    libraries : lib2
)
