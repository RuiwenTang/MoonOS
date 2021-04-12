
umm_malloc = subproject('umm_malloc').get_variable('umm_malloc_dep')

lai_inc = subproject('lai').get_variable('includes')
lai_source = subproject('lai').get_variable('sources')

kernel_headers = [
    'include/moon/acpi.hpp',
    'include/moon/hal.hpp',
    'include/moon/idt.hpp',
    'include/moon/io.hpp',
    'include/moon/memory.h',
    'include/moon/paging.hpp',
    'include/moon/timer.hpp',
    'include/moon/tty.hpp',
    'include/multiboot/multiboot.h',
]

kernel_src = [
    'src/acpi.cc',
    'src/arch/x86_64/entry.S',
    'src/arch/x86_64/idt.S',
    'src/balloc/balloc.cc',
    'src/balloc/balloc.hpp',
    'src/hal.cc',
    'src/idt.cc',
    'src/io.cc',
    'src/kprintf.cc',
    'src/kprintf.hpp',
    'src/main.cc',
    'src/paging.cc',
    'src/runtime.cc',
    'src/timer.cc',
    'src/tty.cc',
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
    '-m64',
    '-T', meson.current_source_dir() + '/link64.ld',
    '-lgcc',
    '-ffreestanding',
    '-nostdlib',
    '-fno-builtin',
    '-z',
    'max-page-size=0x1000'
]

executable('kernel.bin', sources: [srcs, lai_source],
    include_directories: [kernel_incs, lai_inc],
    link_args: kernel_link_args,
    c_args: kernel_c_args,
    cpp_args: [kernel_c_args, kernel_cpp_args],
    dependencies: [klibc_dep, algorithm, umm_malloc]
    )

if get_option('build_test')
    subdir('src/balloc')
endif