

kernel_src = [
    'src/arch/x86_64/entry.S',
    'src/main.cc',
]

srcs = kernel_src

kernel_incs = include_directories('include')

kernel_c_args = [
    '-mno-red-zone',
    '-mno-mmx',
    '-mno-sse',
    '-mno-sse2',
    '-ffreestanding',
    '-mcmodel=kernel',
    '-fno-pic',
    '-Wall',
    '-Wextra',
    '-Wframe-larger-than=1024',
    '-Wstack-usage=1024',
    '-Wno-unknown-warning-option',
    '-nostdlib',
    '-fno-builtin',
    '-m64',
    '-g',
]
kernel_cpp_args = [
    '-fno-exceptions',
    '-std=c++14',
]

add_project_arguments(kernel_c_args, language: ['c', 'cpp'])
add_project_arguments(kernel_cpp_args, language: 'cpp')

kernel_link_args = [
    '-T', meson.current_source_dir() + '/link64.ld',
    '-lgcc',
    '-ffreestanding',
    '-nostdlib',
    '-fno-builtin',
    '-z',
    'max-page-size=0x1000'
]

executable('kernel.bin', sources: srcs, include_directories: kernel_incs, link_args: kernel_link_args)