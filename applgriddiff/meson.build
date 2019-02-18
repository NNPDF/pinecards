project('applgriddiff', 'cpp', default_options : [ 'cpp_std=c++11' ])

# root dependency
root_config = find_program('root-config')
root_cxxflags = run_command(root_config, '--cflags', check : true).stdout().split()
root_ldflags = run_command(root_config, '--libs', check : true).stdout().split()
add_project_arguments(root_cxxflags, language : 'cpp')
add_project_link_arguments(root_ldflags, language : 'cpp')

incdir = include_directories('include')

srcs = [
    'src/diffs.cpp',
    'src/main.cpp'
]

executable('applgriddiff', srcs, include_directories : incdir, install : true)
