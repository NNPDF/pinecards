project('amcblast', 'cpp', version : '2.0.0')

install_headers('amcatnlo_common.h', 'amcfast_interface.h', subdir : 'amcfast')

conf = configuration_data()
conf.set('prefix', get_option('prefix'))
conf.set('includedir', join_paths(get_option('prefix'), get_option('includedir')))
conf.set('exec_prefix', get_option('prefix') + '/')
conf.set('libdir', get_option('libdir'))
conf.set('PACKAGE_VERSION', meson.project_version())

configure_file(
    configuration : conf,
    input : 'amcfast-config.in',
    install_dir : get_option('bindir'),
    output : 'amcfast-config'
)

# this file doesn't need any replacement
configure_file(
    copy : true,
    input : 'amcfast-shower.in',
    install_dir : get_option('bindir'),
    output : 'amcfast-shower'
)

# the header file needs a quoted version string
conf = configuration_data()
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
configure_file(configuration : conf, output : 'amconfig.h')

# root dependency
root_config = find_program('root-config')
root_cxxflags = run_command(root_config, '--cflags', check : true).stdout().split()
root_ldflags = run_command(root_config, '--ldflags', check : true).stdout().split()
add_global_arguments(root_cxxflags, language : 'cpp')
add_global_link_arguments(root_ldflags, language : 'cpp')

# applgrid dependency
applgrid_config = find_program('applgrid-config')
applgrid_cxxflags = run_command(applgrid_config, '--cxxflags', check : true).stdout().split()
applgrid_ldflags = run_command(applgrid_config, '--ldflags', check : true).stdout().split()
add_global_arguments(applgrid_cxxflags, language : 'cpp')
add_global_link_arguments(applgrid_ldflags, language : 'cpp')

# lhapdf dependency
lhapdf = dependency('lhapdf')

lib = static_library(
    'amcfast',
    'amcfast_interface.cxx',
    dependencies : lhapdf,
    install : true,
    pic : true
)

executable('amcfast-psgrid', 'amcfast-psgrid.cxx', install : true, link_with : lib)
