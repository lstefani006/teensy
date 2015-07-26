#!/bin/bash

set -e
set +x

#export PATH=/usr/bin

export V=5.2.0
export B=2.25
export N=2.2.0-1

export MYTOOLS=$PWD/arm-none-eabi-$V
TARGET="--with-cpu=cortex-m4 --with-float=soft --with-mode=thumb"
#TARGET=""

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

if [ ! -e gcc-$V.tar.gz ] ; then
	wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-$V/gcc-$V.tar.gz
fi
if [ ! -e binutils-$B.tar.gz ] ; then
	wget ftp://ftp.gnu.org/gnu/binutils/binutils-$B.tar.gz
fi
if [ ! -e newlib-$N.tar.gz ] ; then
	wget  ftp://sources.redhat.com/pub/newlib/newlib-$N.tar.gz
fi


#######################

if [ ! -e gcc-$V ] ; then
	tar xvf gcc-$V.tar.gz
	pushd gcc-$V
	./contrib/download_prerequisites
	popd
fi

if [ ! -e binutils-$B ] ; then
	tar xvf binutils-$B.tar.gz
	pushd binutils-$B
	../gcc-$V/contrib/download_prerequisites
	popd
fi

if [ ! -e newlib-$N ] ; then
	tar xvf newlib-$N.tar.gz
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
	../../binutils-$B/configure \
		--prefix=$MYTOOLS  \
		--target=arm-none-eabi \
		--disable-nls      \
		--disable-werror   \
		--with-no-thumb-interwork \
		--disable-multilib \
		--with-gnu-as \
		--with-gnu-ls
	make all
	make install
	popd
	touch binutils.ok
fi

if [ ! -e gcc-first.ok ] ; then
	rm -rf gcc
	mkdir gcc
	pushd gcc
	../../gcc-$V/configure \
		--target=arm-none-eabi \
		--prefix=$MYTOOLS \
		--enable-interwork \
		--disable-multilib \
		--disable-werror \
		--enable-languages="c,c++" \
		--with-newlib \
		--without-headers \
		--disable-shared \
		--with-gnu-as \
		--with-gnu-ld \
		-disable-nls \
		$TARGET
	make -j4 all-gcc
	make install-gcc
	popd
	touch gcc-first.ok
fi

export PATH=$MYTOOLS/bin:$PATH

if [ ! -e newlib.ok ] ; then
	rm -rf newlib
	mkdir newlib
	pushd newlib
	../../newlib*/configure \
		--target=arm-none-eabi \
		--prefix=$MYTOOLS \
		--disable-multilib \
		--disable-werror \
		--disable-nls \
		--disable-newlib-supplied-syscalls \
		--with-gnu-ld \
		--with-gnu-as \
		--disable-shared \
		$TARGET
	make CFLAGS_FOR_TARGET="-ffunction-sections -fdata-sections -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fomit-frame-pointer -mcpu=cortex-m4 -mthumb -D__thumb2__ -D__BUFSIZ__=256" \
		CCASFLAGS="-mcpu=cortex-m4 -mthumb -D__thumb2__"
	make install
	popd
	touch newlib.ok
fi

#####################################
if [ ! -e gcc.ok ] ; then
	#rm -rf gcc
	#mkdir gcc
	pushd gcc
	#../../gcc-$V/configure \
	#	--target=arm-none-eabi \
	#	--prefix=$MYTOOLS \
	#	--enable-interwork \
	#	--disable-multilib \
	#	--disable-werror \
	#	--enable-languages="c,c++" \
	#	--with-newlib \
	#	--without-headers \
	#	--disable-shared \
	#	--with-gnu-as \
	#	--with-gnu-ld \
	#	-disable-nls \
	#	$TARGET
	#make -j4 CFLAGS="-mcpu=cortex-m4 -mthumb" CXXFLAGS="-mcpu=cortex-m4 -mthumb" LIBCXXFLAGS="-mcpu=cortex-m4 -mthumb" all
	make -j4 
	make install
	popd
	touch gcc.ok
fi

popd
