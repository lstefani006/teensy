if [ -e $HOME/gcc-site/cortex-m3-gcc-9.3.0 ] ; then
	export PATH=$HOME/gcc-site/cortex-m3-gcc-9.3.0/bin:$PATH
fi

export PATH=$PATH:~/teensy
export PLATFORM=STM32F103
export STM32F103=1

export BMP_PORT=/dev/ttyBmpGdb
export OPENCM3_DIR=$HOME/teensy/LibStm/libopencm3 
export V=1
