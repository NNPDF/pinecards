project(
    'applgriddiff',
    'cpp',
    default_options : [ 'cpp_std=c++14', 'warning_level=3' ],
    meson_version : '>=0.49.0',
    version : '0.0.1'
)

# root dependency
root_dep = dependency('ROOT', method : 'cmake', modules : [ 'ROOT::Core', 'ROOT::Hist' ])

incdir = include_directories('include')

srcs = [
    'src/diffs.cpp',
    'src/main.cpp'
]

executable(
    'applgriddiff',
    srcs,
    include_directories : incdir,
    install : true,
    dependencies : root_dep
)
