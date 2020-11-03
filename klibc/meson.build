
libc_header = [
    'include/stdio.h',
    'include/stdlib.h',
    'include/string.h',
]

libc_src = [
    'src/stdlib/atoi.c',
    'src/string/memchr.c',
    'src/string/memcmp.c',
    'src/string/memcpy.c',
    'src/string/memmove.c',
    'src/string/memset.c',
    'src/string/strcat.c',
    'src/string/strchr.c',
    'src/string/strcmp.c',
    'src/string/strcoll.c',
    'src/string/strcpy.c',
    'src/string/strcspn.c',
    'src/string/strlen.c',
    'src/stdio/snprintf.c',
    'src/stdio/sprintf.c',
    'src/stdio/vsnprintf.c',
    'src/stdio/vsprintf.c',
]

klibc_flags = [
    '-Wall',
    '-Wextra',
    '-Wno-unknown-warning-option',
    '-nostdlib',
    '-fno-builtin',
    '-ffreestanding',
]

klibc = static_library('klibc',
    sources: [libc_header, libc_src],
    include_directories: 'include',
    c_args: klibc_flags
    )

klibc_dep = declare_dependency(
    link_with: klibc,
    include_directories: ['include']
)