#!/bin/bash

set -e
set +x

export MYTOOLS=$PWD/arm-none-eabi-4.9.2
export PATH=$MYTOOLS/bin:$PATH
TARGET="--with-cpu=cortex-m4 --with-float=soft --with-mode=thumb"


OPTIND=1
x=0
while getopts "xh?f:" opt; do
	case "$opt" in
		h|\?)
			echo "usage"
			echo "$0 [-x]"
			echo "-x     clear previous $0 runs"
			exit 0
			;;
		x)
			x=1
			;;
		*)
			echo syntax error
			echo usage:
			echo "$0 [-x]"
			exit 1
	esac
done

if [ $x -gt 0 ] ; then
	rm -rf $MYTOOLS
	rm -f *.ok
	rm -rf build
fi

if [ ! -e gcc-4.9.2.tar.gz ] ; then
	wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-4.9.2/gcc-4.9.2.tar.gz
fi
if [ ! -e binutils-2.25.tar.gz ] ; then
	wget http://ftp.gnu.org/gnu/binutils/binutils-2.25.tar.gz
fi
if [ ! -e newlib-2.2.0.tar.gz ] ; then
	wget  ftp://sources.redhat.com/pub/newlib/newlib-2.2.0.tar.gz
fi


#######################

if [ ! -e gcc-4.9.2 ] ; then
	tar xvf gcc-4.9.2.tar.gz
	pushd gcc-4.9.2
	./contrib/download_prerequisites
	popd
fi

if [ ! -e binutils-2.25 ] ; then
	tar xvf binutils-2.25.tar.gz
	pushd binutils-2.25
	../gcc-4.9.2/contrib/download_prerequisites
	popd
fi

if [ ! -e newlib-2.2.0 ] ; then
	tar xvf newlib-2.2.0.tar.gz
fi

#######################

if [ ! -e build ] ; then
	mkdir -p build
fi
pushd build

#######################
if [ ! -e binutils.ok ] ; then
	rm -rf binutils
	mkdir binutils
	pushd binutils
	../../binutils-2.25/configure \
		--prefix=$MYTOOLS  \
		--disable-werror   \
		--disable-nls      \
		--target=arm-none-eabi \
		--enable-interwork \
		--enable-multilib
	make -j5 all
	make install
	popd
	touch binutils.ok
fi

if [ ! -e gcc-first.ok ] ; then
	rm -rf gcc
	mkdir gcc
	pushd gcc
	../../gcc-4.9.2/configure \
		--target=arm-none-eabi \
		--prefix=$MYTOOLS \
		--enable-interwork \
		--enable-multilib \
		--disable-werror \
		--disable-nls \
		--enable-languages="c,c++" \
		--with-newlib \
		$TARGET
	make -j5 all-gcc
	make install-gcc
	popd
	touch gcc-first.ok
fi

export CFLAGS_FOR_TARGET="\
	-Os \
	-ffunction-sections \
	-fdata-sections \
	-fomit-frame-pointer \
	-DPREFER_SIZE_OVER_SPEED \
	" 
export CFLAGS="-DPREFER_SIZE_OVER_SPEED -DSMALL_MEMORY"


if [ ! -e newlib.ok ] ; then
	rm -rf newlib
	mkdir newlib
	pushd newlib
	../../newlib*/configure  \
		--target=arm-none-eabi   \
		--prefix=$MYTOOLS        \
		--disable-werror         \
		--enable-interwork       \
		--enable-multilib        \
		--disable-nls            \
		--enable-target-optspace \
		--enable-newlib-reent-small        \
		--enable-newlib-io-c99-formats     \
		--enable-newlib-io-long-long       \
		--disable-newlib-multithread       \
		--disable-newlib-supplied-syscalls \
		$TARGET
	make -j5
	make install
	popd
	touch newlib.ok
fi

#####################################
if [ ! -e gcc.ok ] ; then
	rm -rf gcc
	mkdir gcc
	pushd gcc
	../../gcc*/configure \
		--target=arm-none-eabi \
		--prefix=$MYTOOLS \
		--enable-interwork \
		--enable-multilib \
		--disable-werror \
		--disable-nls \
		--enable-languages="c,c++" \
		--with-newlib \
		$TARGET
	make -j5
	make install
	popd
	touch gcc.ok
fi

popd
