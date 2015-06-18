#!/bin/bash

set -e
set +x

export V=5.1.0
export B=2.25
export N=2.2.0-1

export MYTOOLS=$PWD/arm-none-eabi-$V
export PATH=$MYTOOLS/bin:$PATH
#TARGET="--with-cpu=cortex-m4 --with-float=soft --with-mode=thumb"
TARGET=""

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
	wget http://ftp.gnu.org/gnu/binutils/binutils-$B.tar.gz
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
	cat <<EOF > ../../gcc-$V/gcc/config/arm/t-arm-elf
MULTILIB_OPTIONS     = marm/mthumb
MULTILIB_DIRNAMES    = arm thumb
MULTILIB_EXCEPTIONS  = 
MULTILIB_MATCHES     =

MULTILIB_OPTIONS      += march=armv7
MULTILIB_DIRNAMES     += thumb2
MULTILIB_EXCEPTIONS   += march=armv7* marm/*march=armv7*
MULTILIB_EXCEPTIONS   += m*/march=armv7/march*

MULTILIB_MATCHES      += march?armv7=march?armv7-a
MULTILIB_MATCHES      += march?armv7=march?armv7-r
MULTILIB_MATCHES      += march?armv7=march?armv7-m
MULTILIB_MATCHES      += march?armv7=mcpu?cortex-a8
MULTILIB_MATCHES      += march?armv7=mcpu?cortex-a9
MULTILIB_MATCHES      += march?armv7=mcpu?cortex-r4
MULTILIB_MATCHES      += march?armv7=mcpu?cortex-m3
MULTILIB_MATCHES      += march?armv7=mcpu?cortex-m4

# FPv4-SP-D16-M Floating point unit (found on Cortex-M4F)
MULTILIB_OPTIONS      += mfloat-abi=hard mfpu=fpv4-sp-d16
MULTILIB_DIRNAMES     += fpu fpv4spd16
MULTILIB_MATCHES      += mfloat-abi?hard=mhard-float
MULTILIB_EXCEPTIONS   += mfloat* mthumb/mfloat*
MULTILIB_EXCEPTIONS   += mfpu* mthumb/mfpu*
MULTILIB_EXCEPTIONS   += mthumb/march=armv7/mfloat-abi=hard
MULTILIB_EXCEPTIONS   += mthumb/march=armv7/mfpu=fpv4-sp-d16*

MULTILIB_EXCEPTIONS   += mfloat* mno-thumb-interwork/mfloat*
MULTILIB_EXCEPTIONS   += mthumb/mno-thumb-interwork/mfloat-abi=hard
MULTILIB_EXCEPTIONS   += mthumb/mno-thumb-interwork/mfpu=fpv4-sp-d16*
MULTILIB_EXCEPTIONS   += mthumb/mno-thumb-interwork/mfloat-abi=hard/mfpu=fpv4-sp-d16*

#MULTILIB_EXCEPTIONS   += mfloat* mno-thumb-interwork/mfloat*
MULTILIB_EXCEPTIONS   += mno-thumb-interwork/mfloat-abi=hard
MULTILIB_EXCEPTIONS   += mno-thumb-interwork/mfpu=fpv4-sp-d16*
MULTILIB_EXCEPTIONS   += mno-thumb-interwork/mfloat-abi=hard/mfpu=fpv4-sp-d16*

MULTILIB_EXCEPTIONS   += *arm7*mfloat-abi* *arm7*mfpu*
MULTILIB_EXCEPTIONS   += *arm9*mfloat-abi* *arm9*mfpu*
MULTILIB_EXCEPTIONS   += *xscale*mfloat-abi* *xscale*mfpu*
 	
MULTILIB_OPTIONS     += mlittle-endian/mbig-endian
MULTILIB_DIRNAMES    += le be
MULTILIB_MATCHES     += mbig-endian=mbe mlittle-endian=mle
MULTILIB_EXCEPTIONS  += *march=armv7*/*mbig-endian*

# we choose not to build big-endian on armv7 nor arm9/arm9e to save space
MULTILIB_EXCEPTIONS += *mbig-endian*/*mcpu=arm9*
MULTILIB_EXCEPTIONS += *mbig-endian*/*mfloat-abi=hard
MULTILIB_EXCEPTIONS += *mbig-endian*/*mfpu=fpv4-sp-d16
MULTILIB_EXCEPTIONS += *mbig-endian*/*mno-thumb-interwork*/*mfpu=fpv4-sp-d16
MULTILIB_EXCEPTIONS += *mbig-endian*/*mfloat-abi=hard/*mfpu=fpv4-sp-d16

MULTILIB_OPTIONS      += mno-thumb-interwork
MULTILIB_DIRNAMES     += nointerwork
MULTILIB_EXCEPTIONS   += *marm/*mno-thumb-interwork*
MULTILIB_EXCEPTIONS   += *march=armv7*/*mno-thumb-interwork*

MULTILIB_OPTIONS    += mcpu=arm9/mcpu=arm9e/mcpu=xscale
MULTILIB_DIRNAMES   += arm9 arm9e xscale

# Disallow armv7 with any of these alternative cores
MULTILIB_EXCEPTIONS += *march=armv7*/*mcpu=arm7*
MULTILIB_EXCEPTIONS += *march=armv7*/*mcpu=arm9*
MULTILIB_EXCEPTIONS += *march=armv7*/*mcpu=xscale*

# Match relevant arm9 cores
MULTILIB_MATCHES    += mcpu?arm9=mcpu?arm9tdmi
MULTILIB_MATCHES    += mcpu?arm9=mcpu?arm920
MULTILIB_MATCHES    += mcpu?arm9=mcpu?arm920t
MULTILIB_MATCHES    += mcpu?arm9=mcpu?arm922t
MULTILIB_MATCHES    += mcpu?arm9=mcpu?arm940t

# Match relevant arm9e cores (also arm9ej)
MULTILIB_MATCHES    += mcpu?arm9e=mcpu?arm946e-s
MULTILIB_MATCHES    += mcpu?arm9e=mcpu?arm966e-s
MULTILIB_MATCHES    += mcpu?arm9e=mcpu?arm968e-s
MULTILIB_MATCHES    += mcpu?arm9e=mcpu?arm926ej-s
MULTILIB_MATCHES    += mcpu?arm9e=mcpu?arm1026ej-s
EOF
	../../gcc-$V/configure \
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
		CFLAGS="-D__thumb2__"
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
