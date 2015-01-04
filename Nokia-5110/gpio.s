ARM GAS  /tmp/cc9onRo1.s 			page 1


   1              		.syntax unified
   2              		.cpu cortex-m4
   3              		.fpu softvfp
   4              		.eabi_attribute 20, 1
   5              		.eabi_attribute 21, 1
   6              		.eabi_attribute 23, 3
   7              		.eabi_attribute 24, 1
   8              		.eabi_attribute 25, 1
   9              		.eabi_attribute 26, 1
  10              		.eabi_attribute 30, 4
  11              		.eabi_attribute 34, 1
  12              		.eabi_attribute 18, 4
  13              		.thumb
  14              		.file	"gpio.cpp"
  15              		.text
  16              	.Ltext0:
  17              		.cfi_sections	.debug_frame
  18              		.section	.text._Z5Trestv,"ax",%progbits
  19              		.align	1
  20              		.global	_Z5Trestv
  21              		.thumb
  22              		.thumb_func
  24              	_Z5Trestv:
  25              	.LFB3:
  26              		.file 1 "gpio.cpp"
   1:gpio.cpp      **** ﻿
   2:gpio.cpp      **** #include <kinetis.h> 
   3:gpio.cpp      **** 
   4:gpio.cpp      **** /*
   5:gpio.cpp      **** GPIOA_xyz
   6:gpio.cpp      **** GPIOB_xyz
   7:gpio.cpp      **** GPIOC_xyz
   8:gpio.cpp      **** GPIOD_xyz
   9:gpio.cpp      **** 
  10:gpio.cpp      **** PDDR: Port Data Direction Register: 
  11:gpio.cpp      ****       0 means this bit/pin is a input, 1 means output.
  12:gpio.cpp      **** 
  13:gpio.cpp      **** PDIR: Port Data Input Register: 
  14:gpio.cpp      ****       0/1 according to the pin's state.
  15:gpio.cpp      ****       (If any pin is an output it can still be read by PDIR and 
  16:gpio.cpp      **** 	  will simply read back the same value last output on that pin).
  17:gpio.cpp      **** 
  18:gpio.cpp      **** PDOR: Port Data Output Register: 
  19:gpio.cpp      ****       0 sets the bit/pin to 0, 1 to 1.
  20:gpio.cpp      **** 
  21:gpio.cpp      **** The following three sub-registers are logically redundant, 
  22:gpio.cpp      **** but save an instruction or two in some cases.
  23:gpio.cpp      **** 
  24:gpio.cpp      **** PSOR: Port Set Output Register: 0 has no effect. 
  25:gpio.cpp      ****       1 sets the corresponding bit/pin to 1.
  26:gpio.cpp      **** 
  27:gpio.cpp      **** PCOR: Port Clear Output Register: 0 has no effect. 
  28:gpio.cpp      ****       1 sets the corresponding bit/pin to 0.
  29:gpio.cpp      **** 
  30:gpio.cpp      **** PTOR: Port Toggle Output Register: 0 has no effect. 
  31:gpio.cpp      ****       1 toggles the corresponding bit/pin.
  32:gpio.cpp      **** 
ARM GAS  /tmp/cc9onRo1.s 			page 2


  33:gpio.cpp      **** Teensy3 pin / GPIOx / bit in GPIOx
  34:gpio.cpp      ****  0 B 16
  35:gpio.cpp      ****  1 B 17
  36:gpio.cpp      ****  2 D  0
  37:gpio.cpp      ****  3 A 12
  38:gpio.cpp      ****  4 A 13
  39:gpio.cpp      ****  5 D  7
  40:gpio.cpp      ****  6 D  4
  41:gpio.cpp      ****  7 D  2
  42:gpio.cpp      ****  8 D  3
  43:gpio.cpp      ****  9 C  3
  44:gpio.cpp      **** 10 C  4
  45:gpio.cpp      **** 11 C  6
  46:gpio.cpp      **** 12 C  7
  47:gpio.cpp      **** 13 C  5
  48:gpio.cpp      **** 14 D  1
  49:gpio.cpp      **** 15 C  0
  50:gpio.cpp      **** 16 B  0
  51:gpio.cpp      **** 17 B  1
  52:gpio.cpp      **** 18 B  3
  53:gpio.cpp      **** 19 B  2
  54:gpio.cpp      **** 20 D  5
  55:gpio.cpp      **** 21 D  6
  56:gpio.cpp      **** 22 C  1
  57:gpio.cpp      **** 23 C  2
  58:gpio.cpp      **** 24 A  5
  59:gpio.cpp      **** 25 B 19
  60:gpio.cpp      **** 26 E  1
  61:gpio.cpp      **** 27 C  9
  62:gpio.cpp      **** 28 C  8
  63:gpio.cpp      **** 29 C 10
  64:gpio.cpp      **** 30 C 11
  65:gpio.cpp      **** 31 E  0
  66:gpio.cpp      **** 32 B 18
  67:gpio.cpp      **** 33 A  4
  68:gpio.cpp      **** */
  69:gpio.cpp      **** 
  70:gpio.cpp      **** class GPIOA {
  71:gpio.cpp      **** public:
  72:gpio.cpp      **** 	enum Direction { In=0, Out=1 };
  73:gpio.cpp      **** 	static void SetDirection(int pin, Direction dir) {
  74:gpio.cpp      **** 		if (dir == In)
  75:gpio.cpp      **** 			GPIOA_PDDR &= ~(1 << pin);
  76:gpio.cpp      **** 		else
  77:gpio.cpp      **** 			GPIOA_PDDR |= (1 << pin);
  78:gpio.cpp      **** 	}
  79:gpio.cpp      **** 	static void Write(int pin, bool v) {
  80:gpio.cpp      **** 		if (v)
  81:gpio.cpp      **** 			GPIOA_PSOR = (1 << pin);
  82:gpio.cpp      **** 		else
  83:gpio.cpp      **** 			GPIOA_PCOR = (1 << pin);
  84:gpio.cpp      **** 	}
  85:gpio.cpp      **** 	static bool Read(int pin) {
  86:gpio.cpp      **** 		return (GPIOA_PDIR & (1 << pin)) ? true : false;
  87:gpio.cpp      **** 	}
  88:gpio.cpp      **** };
  89:gpio.cpp      **** 
ARM GAS  /tmp/cc9onRo1.s 			page 3


  90:gpio.cpp      **** bool Trest() {
  27              		.loc 1 90 0
  28              		.cfi_startproc
  29              		@ args = 0, pretend = 0, frame = 0
  30              		@ frame_needed = 0, uses_anonymous_args = 0
  31              		@ link register save eliminated.
  32              	.LVL0:
  33              	.LBB14:
  34              	.LBB15:
  75:gpio.cpp      **** 		else
  35              		.loc 1 75 0
  36 0000 084B     		ldr	r3, .L2
  37              	.LBE15:
  38              	.LBE14:
  39              	.LBB17:
  40              	.LBB18:
  81:gpio.cpp      **** 		else
  41              		.loc 1 81 0
  42 0002 0949     		ldr	r1, .L2+4
  43              	.LBE18:
  44              	.LBE17:
  45              	.LBB20:
  46              	.LBB16:
  75:gpio.cpp      **** 		else
  47              		.loc 1 75 0
  48 0004 1A68     		ldr	r2, [r3]
  49 0006 22F00802 		bic	r2, r2, #8
  50 000a 1A60     		str	r2, [r3]
  51              	.LVL1:
  52              	.LBE16:
  53              	.LBE20:
  54              	.LBB21:
  55              	.LBB22:
  83:gpio.cpp      **** 	}
  56              		.loc 1 83 0
  57 000c 074A     		ldr	r2, .L2+8
  58              	.LBE22:
  59              	.LBE21:
  60              	.LBB24:
  61              	.LBB19:
  81:gpio.cpp      **** 		else
  62              		.loc 1 81 0
  63 000e 0823     		movs	r3, #8
  64 0010 0B60     		str	r3, [r1]
  65              	.LVL2:
  66              	.LBE19:
  67              	.LBE24:
  68              	.LBB25:
  69              	.LBB23:
  83:gpio.cpp      **** 	}
  70              		.loc 1 83 0
  71 0012 1360     		str	r3, [r2]
  72              	.LVL3:
  73              	.LBE23:
  74              	.LBE25:
  75              	.LBB26:
  76              	.LBB27:
ARM GAS  /tmp/cc9onRo1.s 			page 4


  81:gpio.cpp      **** 		else
  77              		.loc 1 81 0
  78 0014 0B60     		str	r3, [r1]
  79              	.LVL4:
  80              	.LBE27:
  81              	.LBE26:
  82              	.LBB28:
  83              	.LBB29:
  83:gpio.cpp      **** 	}
  84              		.loc 1 83 0
  85 0016 1360     		str	r3, [r2]
  86              	.LVL5:
  87              	.LBE29:
  88              	.LBE28:
  89              	.LBB30:
  90              	.LBB31:
  86:gpio.cpp      **** 	}
  91              		.loc 1 86 0
  92 0018 054B     		ldr	r3, .L2+12
  93 001a 1868     		ldr	r0, [r3]
  94              	.LBE31:
  95              	.LBE30:
  91:gpio.cpp      **** 	GPIOA::SetDirection(3, GPIOA::In);
  92:gpio.cpp      **** 	GPIOA::Write(3, true);
  93:gpio.cpp      **** 	GPIOA::Write(3, false);
  94:gpio.cpp      **** 	GPIOA::Write(3, true);
  95:gpio.cpp      **** 	GPIOA::Write(3, false);
  96:gpio.cpp      **** 	return GPIOA::Read(3);
  97:gpio.cpp      **** }
  96              		.loc 1 97 0
  97 001c C0F3C000 		ubfx	r0, r0, #3, #1
  98 0020 7047     		bx	lr
  99              	.L3:
 100 0022 00BF     		.align	2
 101              	.L2:
 102 0024 14F00F40 		.word	1074786324
 103 0028 04F00F40 		.word	1074786308
 104 002c 08F00F40 		.word	1074786312
 105 0030 10F00F40 		.word	1074786320
 106              		.cfi_endproc
 107              	.LFE3:
 109              		.text
 110              	.Letext0:
 111              		.file 2 "/home/leo/arm-none-eabi/arm-none-eabi/include/machine/_default_types.h"
 112              		.file 3 "/home/leo/arm-none-eabi/arm-none-eabi/include/stdint.h"
ARM GAS  /tmp/cc9onRo1.s 			page 5


DEFINED SYMBOLS
                            *ABS*:00000000 gpio.cpp
     /tmp/cc9onRo1.s:19     .text._Z5Trestv:00000000 $t
     /tmp/cc9onRo1.s:24     .text._Z5Trestv:00000000 _Z5Trestv
     /tmp/cc9onRo1.s:102    .text._Z5Trestv:00000024 $d
                     .debug_frame:00000010 $d

NO UNDEFINED SYMBOLS
