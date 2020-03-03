# we need the python modules `breathe` and `sphinx_rtd_theme` to build the documentation
import('python').find_installation('python3', modules : [ 'breathe', 'sphinx_rtd_theme' ])

conf = configuration_data({
    'builddir'        : meson.current_build_dir(),
    'PACKAGE_NAME'    : meson.project_name(),
    'PACKAGE_VERSION' : meson.project_version(),
    'srcdir'          : meson.current_source_dir(),
    'top_srcdir'      : meson.source_root(),
    'xmldir'          : 'xml',
    'htmldir'         : 'html',
})

run_doxygen_breathe = configure_file(
    configuration : conf,
    input : 'run_doxygen_breathe.py.in',
    output : 'run_doxygen_breathe.py'
)

doxygen = find_program('doxygen')
sphinx_build = find_program('sphinx-build')

doc_srcs = [
    'index.rst',
    'luminosity.rst',
    'reference.rst',
]

documentation = custom_target(
    'doxygen-breathe',
    build_by_default : true,
    command : [ run_doxygen_breathe, doxygen, sphinx_build, '@OUTPUT@' ],
    depend_files : [ headers, doc_srcs ],
    output : [ conf.get('xmldir'), conf.get('htmldir') ]
)

install_subdir(
    meson.current_build_dir() / conf.get('htmldir'),
    exclude_directories : '.doctrees',
    exclude_files : '.buildinfo',
    install_dir : get_option('datadir') / 'doc' / meson.project_name(),
    strip_directory : true
)
