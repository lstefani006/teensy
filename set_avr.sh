#export PATH=$PATH:$HOME/arduino-1.6.11/hardware/tools/avr/bin
#export ARDUINO=$HOME/arduino-1.6.11

export PATH=$PATH:$HOME/gcc-site/avr-gcc-6.2.0/bin:/home/leo/teensy/AvrLib
if [ -e $HOME/arduino-1.6.11 ] ; then
	export ARDUINO=$HOME/arduino-1.6.11
fi
if [ -e $HOME/arduino-1.6.12 ] ; then
	export ARDUINO=$HOME/arduino-1.6.12
fi
export PLATFORM=AVR
