#export PATH=$PATH:$HOME/arduino-1.6.11/hardware/tools/avr/bin

if [ -e $HOME/arduino-1.8.3 ] ; then
	export ARDUINO=$HOME/arduino-1.8.3
elif [ -e $HOME/arduino-1.8.2 ] ; then
	export ARDUINO=$HOME/arduino-1.8.2
elif [ -e $HOME/arduino-1.6.12 ] ; then
	export ARDUINO=$HOME/arduino-1.6.12
elif [ -e $HOME/arduino-1.6.11 ] ; then
	export ARDUINO=$HOME/arduino-1.6.11
fi

echo ARDUINO=$ARDUINO

########################################
# per lanciare e stoppare ArduinoSerialMonitor.exe
export PATH=$PATH:$HOME/teensy

########################################

if [ -e $HOME/gcc-site/avr-gcc-7.3.0 ] ; then
	export PATH=$PATH:$HOME/gcc-site/avr-gcc-7.3.0/bin
elif [ -e $HOME/gcc-site/avr-gcc-7.2.0 ] ; then
	export PATH=$PATH:$HOME/gcc-site/avr-gcc-7.2.0/bin
elif [ -e $HOME/gcc-site/avr-gcc-7.1.0 ] ; then
	export PATH=$PATH:$HOME/gcc-site/avr-gcc-7.1.0/bin
elif [ -e $HOME/gcc-site/avr-gcc-6.2.0 ] ; then
	export PATH=$PATH:$HOME/gcc-site/avr-gcc-6.2.0/bin
fi
echo PATH=$PATH

########################################
export PLATFORM=AVR
