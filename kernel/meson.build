
kernel_headers = [
    'include/moon/hal.hpp',
    'include/moon/memory.h',
    'include/moon/tty.hpp',
    'include/moon/idt.hpp',
    'include/moon/io.hpp',
    'include/moon/paging.hpp',
    'include/multiboot/multiboot.h',
]

kernel_src = [
    'src/arch/x86_64/entry.S',
    'src/arch/x86_64/idt.S',
    'src/main.cc',
    'src/tty.cc',
    'src/idt.cc',
    'src/io.cc',
    'src/hal.cc',
    'src/paging.cc',
    'src/runtime.cc',
    'src/kprintf.hpp',
    'src/kprintf.cc',
    'src/balloc/balloc.hpp',
    'src/balloc/balloc.cc',
]

srcs = [kernel_src, kernel_headers ]

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


kernel_link_args = [
    '-T', meson.current_source_dir() + '/link64.ld',
    '-lgcc',
    '-ffreestanding',
    '-nostdlib',
    '-fno-builtin',
    '-z',
    'max-page-size=0x1000'
]

executable('kernel.bin', sources: srcs,
    include_directories: kernel_incs,
    link_args: kernel_link_args,
    c_args: kernel_c_args,
    cpp_args: [kernel_c_args, kernel_cpp_args],
    dependencies: [klibc_dep, algorithm]
    )