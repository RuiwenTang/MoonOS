#! /bin/bash

export PREFIX="$HOME/osdev/toolchain"
export TARGET=x86_64-moonos-elf
export PATH="$PREFIX/bin:$PATH"
export CROSS_SYSROOT="$HOME/osdev/sysroot"

mkdir -p toolchain

cd toolchain

TOOLCHAIN_ROOT=$pwd

# if [ ! -f binutils-2.34.tar.gz ]
# then
#     wget http://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.gz
# fi

# if [ ! -d binutils-2.34 ]
# then
# tar xvfz binutils-2.34.tar.gz
# fi

# if [ ! -d build_binutils ]
# then

# mkdir build_binutils
# cd build_binutils
# ../binutils-2.34/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot=$CROSS_SYSROOT --disable-werror
# make
# make install
# cd ../
# fi

# if [ ! -f gcc-9.3.0.tar.xz ]
# then
#     wget ftp://ftp.gnu.org/gnu/gcc/gcc-9.3.0/gcc-9.3.0.tar.xz
# fi

# if [ ! -d gcc-9.3.0 ]
# then
#     tar xvfz gcc-9.3.0.tar.xz
#     cd gcc-9.3.0
#     ./contrib/download_prerequisites
#     cd ../
# fi

# if [ ! -d build_gcc ]
# then
#     mkdir -p build_gcc
#     cd build_gcc
#     echo $pwd
#     ../gcc-9.3.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++
#     make all-gcc
#     make all-target-libgcc
#     make install-gcc
#     make install-target-libgcc
# fi

if [ ! -f gdb-8.0.tar.gz ]
then
    wget ftp://ftp.gnu.org/gnu/gdb/gdb-8.0.tar.gz
fi

if [ ! -d gdb-8.0 ]
then
    tar xvfz gdb-8.0.tar.gz
fi

if [ ! -d build_gdb ]
then
    mkdir -p build_gdb
    cd build_gdb
    echo $pwd
    ../gdb-8.0/configure --target=x86_64-elf --prefix="$PREFIX" --disable-nls --disable-werror
    make
    make install
fi
