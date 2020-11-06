

algorithm = declare_dependency(
    include_directories: ['.'],
    version: '0.0.1',
    compile_args: ['-std=c++14'],
)

if get_option('build_test')

    executable('list_test', native: true, sources: ['test/list_test.cc'], dependencies: [algorithm])

endif