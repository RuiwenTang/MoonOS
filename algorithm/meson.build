

algorithm = declare_dependency(
    include_directories: ['.'],
    version: '0.0.1',
    compile_args: ['-std=c++14'],
)

if get_option('build_test')
    gtest = subproject('gtest').get_variable('gtest_dep')
    executable('algorithm_test', native: true,
        sources: [
            'test/list_test.cc',
            'test/test_main.cc',
           ], dependencies: [algorithm, gtest])

endif