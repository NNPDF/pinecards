conf = configuration_data()
conf.set('PACKAGE_NAME', meson.project_name())
conf.set('PACKAGE_VERSION', meson.project_version())

conf_py = configure_file(configuration : conf, input : 'conf.py.in', output : '@BASENAME@')

# we need the python module `sphinx_rtd_theme` to build the documentation
import('python').find_installation('python3', modules : 'sphinx_rtd_theme')

sphinx_build = find_program('sphinx-build')

doc_srcs = [
    'index.rst'
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
