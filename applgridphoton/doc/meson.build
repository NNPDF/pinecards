conf = configuration_data({
    'builddir'        : meson.current_build_dir(),
    'PACKAGE_NAME'    : meson.project_name(),
    'PACKAGE_VERSION' : meson.project_version(),
    'top_srcdir'      : meson.source_root(),
})

doxygen = find_program('doxygen')

doxyfile = configure_file(
    configuration : conf,
    input : 'Doxyfile.in',
    output : 'Doxyfile'
)

doxygen_xml = custom_target(
    'doxygen-xml',
    command : [ doxygen, doxyfile ],
    depend_files : [ headers ],
    input : doxyfile,
    output : 'xml'
)

conf_py = configure_file(configuration : conf, input : 'conf.py.in', output : '@BASENAME@')

# we need the python modules `breathe` and `sphinx_rtd_theme` to build the documentation
import('python').find_installation('python3', modules : [ 'breathe', 'sphinx_rtd_theme' ])

sphinx_build = find_program('sphinx-build')

doc_srcs = [
    'index.rst',
    'luminosity.rst',
    'reference.rst',
]

custom_target(
    'documentation',
    build_by_default : true,
    command : [
        sphinx_build,
        '-n',
        '-q',
        # read the generated `conf.py` from the build directory
        '-c', meson.current_build_dir(),
        # this is where the `*.rst` sources are
        meson.current_source_dir(),
        # write all the output files in a subdirectory, so we can easily clean it
        meson.current_build_dir() / 'sphinx_build'
    ],
    depend_files : conf_py,
    depends : doxygen_xml,
    input : doc_srcs,
    output : 'sphinx_build'
)

install_subdir(
    meson.current_build_dir() / 'sphinx_build',
    exclude_directories : '.doctrees',
    exclude_files : '.buildinfo',
    install_dir : get_option('datadir') / 'doc' / meson.project_name(),
    strip_directory : true
)
