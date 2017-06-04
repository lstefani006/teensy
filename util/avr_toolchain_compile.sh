#!/bin/bash

set -e
set -x


function download_latest_version() {
	a=$(wget -q -O- $1 | egrep -o "$2$3$4" | sort -V | tail -1)
	wget $1/$a
}

function get_latest_version() {
	a=$(wget -q -O- $1 | egrep -o "$2$3$4" | sort -V | tail -1)
	if [[ "$a" =~ $2($3)$4 ]]; then
		echo ${BASH_REMATCH[1]}
	else
		echo ERROR
	fi
}

export V=$(get_latest_version ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/ gcc-       [0-9\.]+     "")
export B=$(get_latest_version http://ftp.gnu.org/gnu/binutils                     binutils-  [0-9\.]+     .tar.gz)
export A=$(get_latest_version  http://download.savannah.gnu.org/releases/avrdude  avrdude-   [0-9\.]+     .tar.gz)
export G=$(get_latest_version http://ftp.gnu.org/gnu/gdb                          gdb-       [0-9\.]+     .tar.gz)
export N=$(get_latest_version http://download.savannah.gnu.org/releases/avr-libc  avr-libc-  [0-9\.]+     .tar.bz2)

export AVR_DUDE=$A

#export V=7.1.0
#export B=2.28
#export N=2.0.0
#export AVR_DUDE=6.3

export MYTOOLS=$PWD/avr-gcc-$V
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
	wget ftp://ftp.gnu.org/gnu/binutils/binutils-$B.tar.gz
fi
if [ ! -e avr-size.patch ] ; then
	wget https://slackbuilds.org/cgit/slackbuilds/plain/development/avr-binutils/avr-size.patch
fi
if [ ! -e avr-libc-$N.tar.bz2 ] ; then
	wget http://download.savannah.gnu.org/releases/avr-libc/avr-libc-$N.tar.bz2
fi
if [ ! -e avrdude-$AVR_DUDE.tar.gz ] ; then
	wget http://download.savannah.gnu.org/releases/avrdude/avrdude-$AVR_DUDE.tar.gz
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
	pushd binutils-$B/binutils
	patch size.c ../../avr-size.patch
	popd 
fi

if [ ! -e avr-libc-$N ] ; then
	bunzip2 -c avr-libc-$N.tar.bz2 | tar xf -
fi

if [ ! -e avrdude-$AVR_DUDE ] ; then
	tar xvf avrdude-$AVR_DUDE.tar.gz
fi

#######################

if [ ! -e build ] ; then
	mkdir -p build
fi
pushd build

#######################
if [ ! -e avrdude.ok ] ; then
	rm -rf avrdude
	mkdir avrdude
	pushd avrdude
	../../avrdude-$AVR_DUDE/configure \
		--prefix=$MYTOOLS 
	make
	make install
	popd
	touch avrdude.ok
fi

if [ ! -e binutils.ok ] ; then
	rm -rf binutils
	mkdir binutils
	pushd binutils
	../../binutils-$B/configure \
		--prefix=$MYTOOLS  \
		--target=avr \
		--disable-nls      \
		$TARGET
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
		--prefix=$MYTOOLS \
		--target=avr \
		--enable-languages="c,c++" \
		-disable-nls \
		--disable-libssp \
		--with-dwarf2 \
		--without-headers \
		$TARGET
	make -j4 all-gcc
	make install-gcc
	popd
	touch gcc-first.ok
fi

export PATH=$MYTOOLS/bin:$PATH

if [ ! -e avr-libc.ok ] ; then
	rm -rf avr-libc
	mkdir avr-libc
	pushd avr-libc
	../../avr-libc*/configure \
		--prefix=$MYTOOLS \
		--build=`./config.guess` \
		--host=avr \
		--with-debug-info=DEBUG_INFO
	make 
	make install
	popd
	touch avr-libc.ok
fi

#####################################
if [ ! -e gcc.ok ] ; then
	#rm -rf gcc
	#mkdir gcc
	pushd gcc
	../../gcc-$V/configure \
		--prefix=$MYTOOLS \
		--target=avr \
		--enable-languages="c,c++" \
		-disable-nls \
		--disable-libssp \
		--with-dwarf2 \
		--without-headers \
		$TARGET
	make -j4 all
	make install
	popd
	touch gcc.ok
fi

popd
