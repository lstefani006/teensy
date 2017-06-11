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
export N=$(get_latest_version ftp://sourceware.org/pub/newlib/index.html          newlib-    [0-9\.]+     .tar.gz)
export A=$(get_latest_version  http://download.savannah.gnu.org/releases/avrdude  avrdude-   [0-9\.]+     .tar.gz)
export G=$(get_latest_version http://ftp.gnu.org/gnu/gdb                          gdb-       [0-9\.]+     .tar.gz)

# qui si imposta la cpu....
#CPU="cortex-m4"   # teensy
CPU="cortex-m3"    # st32m103c

PREFIX=$PWD/$CPU-gcc-$V
TARGET=arm-none-eabi
CONFIGURE="--with-cpu=$CPU --with-float=soft --with-mode=thumb"

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
	rm -rf $PREFIX
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
	#wget  ftp://sources.redhat.com/pub/newlib/newlib-$N.tar.gz
	wget ftp://sourceware.org/pub/newlib/newlib-$N.tar.gz
fi
if [ ! -e gdb-$G.tar.gz ] ; then
	wget  ftp://ftp.gnu.org/gnu/gdb/gdb-$G.tar.gz
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
fi

if [ ! -e newlib-$N ] ; then
	tar xvf newlib-$N.tar.gz
fi

if [ ! -e gdb-$G ] ; then
	tar xvf gdb-$G.tar.gz
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
	../../binutils-$B/configure   \
		--prefix=$PREFIX          \
		--target=$TARGET          \
		--disable-nls             \
		--disable-werror          \
		--with-no-thumb-interwork \
		--disable-multilib        \
		--with-gnu-as             \
		--with-gnu-ls             \
		$CONFIGURE
	make all
	make install
	popd
	touch binutils.ok
fi

if [ ! -e gcc-first.ok ] ; then
	rm -rf gcc
	mkdir gcc
	pushd gcc
	../../gcc-$V/configure     \
		--target=$TARGET       \
		--prefix=$PREFIX       \
		--enable-interwork     \
		--disable-multilib     \
		--disable-werror       \
		--enable-languages="c" \
		--with-newlib          \
		--without-headers      \
		--disable-shared       \
		--with-gnu-as          \
		--with-gnu-ld          \
		-disable-nls           \
		$CONFIGURE
	make -j2 all-gcc
	make install-gcc
	popd
	touch gcc-first.ok
fi

export PATH=$PREFIX/bin:$PATH

CFLAGS_FOR_TARGET="\
	-mcpu=$CPU -mthumb -D__thumb2__              \
	-ffunction-sections -fdata-sections          \ # put code and data into separate sections allowing for link-time
	-DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ \ # pick simpler, smaller code over larger optimized code
	-Os                                          \ # same as O2, but turns off optimizations that would increase code size
	-fomit-frame-pointer                         \ # don't keep the frame pointer in a register for functions that don't need one
	-fno-unroll-loops                            \ # don't unroll loops
	-D__BUFSIZ__=256"                              # limit default buffer size to 256 rather than 1024

CCASFLAGS=$CFLAGS_FOR_TARGET

if [ ! -e newlib.ok ] ; then
	rm -rf newlib
	mkdir newlib
	pushd newlib
	../../newlib*/configure    \
		--target=$TARGET       \
		--prefix=$PREFIX       \
		--disable-multilib     \
		--disable-werror       \
		--disable-nls          \
		--disable-newlib-supplied-syscalls \
		--with-gnu-ld          \
		--with-gnu-as          \
		--disable-shared       \
		$CONFIGURE
	make CFLAGS_FOR_TARGET=$(CFLAGS_FOR_TARGET) CCASFLAGS=$(CCASFLAGS)

	make install
	popd
	touch newlib.ok
fi

#####################################
if [ ! -e gcc.ok ] ; then
	#rm -rf gcc
	#mkdir gcc
	pushd gcc
	../../gcc-$V/configure     \
		--target=$TARGET       \
		--prefix=$PREFIX       \
		--enable-interwork     \
		--disable-multilib     \
		--disable-werror       \
		--enable-languages="c,c++" \
		--with-newlib          \
		--disable-shared       \
		--with-gnu-as          \
		--with-gnu-ld          \
		-disable-nls           \
		$CONFIGURE
	make -j2 all
	make install
	popd
	touch gcc.ok
fi

#####################################
if [ ! -e gdb.ok ] ; then
	rm -rf gdb
	mkdir  gdb
	pushd  gdb
	../../gdb*/configure    \
		--target=$TARGET       \
		--prefix=$PREFIX       \
		--disable-multilib     \
		--disable-werror       \
		--disable-nls          \
		--disable-newlib-supplied-syscalls \
		--with-gnu-ld          \
		--with-gnu-as          \
		--disable-shared       \
		--with-guile=no        \
		$CONFIGURE
	make all
	make install
	popd
	touch gdb.ok
fi

popd
