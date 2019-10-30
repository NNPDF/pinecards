config = configuration_data()
config.set('prefix', get_option('prefix'))
config.set('includedir', get_option('prefix') / get_option('includedir'))
config.set('ROOTCFLAGS', '')
config.set('exec_prefix', get_option('prefix') + '/')
config.set('libdir', get_option('libdir'))
config.set('ROOTLDFLAGS', '')
config.set('HOPPETLIBS', '')
config.set('datarootdir', get_option('prefix') / get_option('datadir'))
config.set('PACKAGE', meson.project_name())
config.set('PACKAGE_VERSION', meson.project_version())

conf_bin = configure_file(
    configuration : config,
    input : 'applgrid-config.in',
    output : 'applgrid-config',
    install : true,
    install_dir : get_option('bindir'),
    install_mode : 'rwxr-xr-x'
)
