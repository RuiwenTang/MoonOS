
executable('balloc_test', native: true,
    sources: [
        'balloc_test.cc',
        'balloc.cc',
        'balloc.hpp',
        ],
        dependencies: [algorithm],
        include_directories: ['.'],
        cpp_args: ['-DHOST_BUILD'],
        compile_args: ['-std=c++14'],
)