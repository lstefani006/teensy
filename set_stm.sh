
if [ -e $HOME/gcc-site/cortex-m3-gcc-7.1.0 ] ; then
	export PATH=$PATH:$HOME/gcc-site/cortex-m3-gcc-7.1.0/bin
fi

export PATH=$PATH:~/teensy
export PLATFORM=STM32F103
export STM32F103=1

export BMP_PORT=/dev/ttyBmpGdb
export OPENCM3_DIR=$HOME/teensy/LibStm/libopencm3 
export V=1