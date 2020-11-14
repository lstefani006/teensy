#include <string.h>
#include <stdlib.h>

#include "ssd1306_i2c.h"
#include "i2c_utils.hpp"
#include "fonts.hpp"

#define I2C_COMMAND 0x00
#define I2C_DATA 0x40

// Fundamental Command Table
#define SSD1306_SET_CONTROL 0x81 // Double byte command to select 1 out of 256 contrast steps. Contrast increases as the value increases.
#define SSD1306_RESET 0x7F
#define SSD1306_DISPLAY_ON_RAM 0xA4	 // Resume to RAM content display (RESET)
#define SSD1306_DISPLAY_NO_RAM 0xA5	 // Output ignores RAM content
#define SSD1306_SET_NORMAL 0xA6		 // Normal display (RESET)
#define SSD1306_SET_INVERSE 0xA7	 // Inverse display
#define SSD1306_SET_DISPLAY_OFF 0xAE // Display OFF (sleep mode)
#define SSD1306_SET_DISPLAY_ON 0xAF	 // Display ON in normal mode

// Addressing mode
#ifndef DEFAULTBUFFERLENGTH
#define DEFAULTBUFFERLENGTH 1024
#endif

#define DATAONLY (uint8_t)0b01000000
#define COMMAND (uint8_t)0b00000000

SD1306::SD1306(ESP32_I2C &i2c, int width, int height)
	: i2c(i2c)
{
	_WIDTH = width;
	_HEIGHT = height;
	_screenBufferLength = width * height / 8;
	_screenRAM = (uint8_t *)malloc(_screenBufferLength);

	_mutex_cx = 0;
	_mutex_cy = 0;
	_mutex_font = font_08x08;
}

/*
SD1306::~SD1306()
{
	if (_screenRAM)
		free(_screenRAM);
}
*/

/* Device memory organised in 128 horizontal pixel and up to 8 rows of byte
 *
	 B  B .............B  -----
	 y  y .............y        \
	 t  t .............t         \
	 e  e .............e          \
	 0  1 .............127         \
	                                \
	 D0 D0.............D0  LSB       \  ROW 0  |
	 D1 D1.............D1            /  ROW 7  V
	 D2 D2.............D2   |       /
	 D3 D3.............D3   |      /
	 D4 D4.............D4   V     /
	 D5 D5.............D5        /
	 D6 D6.............D6  MSB  /
	 D7 D7.............D7  ----

*/

/** I2C bus write data
 *
 * According to datasheet (8.1.5.1) the I2C-bus interface gives access to write data and command into the device. Please refer to Figure for
 * the write mode of I2C-bus in chronological order.
 *
 * S
 * [011110(SA)(r/w)] ack
 * [(Co)(D/C)(Control bites(6))] (ack) [Data byte] (ack)   \
 * [(Co)(D/C)(Control bites(6))] (ack) [Data byte] (ack)    - m >= 0 words
 * [(Co)(D/C)(Control bites(6))] (ack) [Data byte] (ack)   /
 * [(Co)(D/C)(Control bites(6))] (ack) [Data byte] (ack)   every [...] -- 1 byte
 * [(Co)(D/C)(Control bites(6))] (ack) [Data byte] (ack)   [Data byte] -- should be n >= 0
 * [(Co)(D/C)(Control bites(6))] (ack) [Data byte] (ack)   MSB.....LSB
 * [Data byte] (ack) [Data byte] (ack) [Data byte] (ack)
 * [Data byte] (ack) [Data byte] (ack) [Data byte] (ack)
 * P
 *
 * [011110(SA)(r/w)]                -- SSD1306 slave address
 * [(Co)(D/C)(Control bites(6))]    -- Control byte
 *
 * 1. The master device initiates the data communication by a start condition.
 *    The start condition is established by pulling the SDA from HIGH to LOW while the SCL stays HIGH.
 * 2. The slave address is following the start condition for recognition use. For the SSD1306, the slave
 *      address is either “b0111100” or “b0111101” by changing the SA0 to LOW or HIGH (D/C pin acts as
 *      SA0).
 * 3. The write mode is established by setting the R/W# bit to logic “0”.
 * 4. An acknowledgement signal will be generated after receiving one byte of data, including the slave
 *      address and the R/W# bit. The acknowledge bit is defined as the SDA line is pulled down during the HIGH
 *      period of the acknowledgement related clock pulse.
 * 5. After the transmission of the slave address, either the control byte or the data byte may be sent across
 *      the SDA. A control byte mainly consists of Co and D/C# bits following by six “0” ‘s.
 *          a. If the Co bit is set as logic “0”, the transmission of the following information will contain
 *             data _bytes_ only.
 *          b. The D/C# bit determines the next data byte is acted as a command or a data. If the D/C# bit is
 *             set to logic “0”, it defines the following data byte as a command. If the D/C# bit is set to
 *             logic “1”, it defines the following data byte as a data which will be stored at the GDDRAM.
 *              The GDDRAM column address pointer will be increased by one automatically after each
 *              data write.
 * 6. Acknowledge bit will be generated after receiving each control byte or data byte.
 * 7. The write mode will be finished when a stop condition is applied. The stop condition is also defined
 *      in Figure 8-8. The stop condition is established by pulling the “SDA in” from LOW to HIGH while
 *      the “SCL” stays HIGH.
 *
 *      @param spec -- should be DATAONLY or COMMAND
 *
 *      Note: if you choose DATAONLY then a few bytes CAN follow that
 *      Note(2): In fact it doesn't really matter what that datasheet has got written in. Every command can be sended
 *      only one way: with set address and begin i2c transmission for EVERY single byte. It means that if you have
 *      3 byte command you MUST send each of them separatelly byte per byte in start->address->cmd-zero->byte->ack sequences.
 */

void SD1306::send_data(uint8_t spec, uint8_t data)
{
	uint8_t b[2];
	b[0] = spec;
	b[1] = data;

	i2c.Write(b, 2);
}

/**
  *  According Reset Circuit
  *  When RES# input is LOW, the chip is initialized with the following status:
  *  1. Display is OFF
  *  2. 128 x 64 Display Mode
  *  3. Normal segment and display data column address and row address mapping (SEG0 mapped to
  *      address 00h and COM0 mapped to address 00h)
  *  4. Shift register data clear in serial interface
  *  5. Display start line is set at display RAM address 0
  *  6. Column address counter is set at 0
  *  7. Normal scan direction of the COM outputs
  *  8. Contrast control register is set at 7Fh
  *  9. Normal display mode (Equivalent to A4h command)
  */

#define SSD1306_MEMORYMODE 0x20			 ///< See datasheet
#define SSD1306_COLUMNADDR 0x21			 ///< See datasheet
#define SSD1306_PAGEADDR 0x22			 ///< See datasheet
#define SSD1306_SETCONTRAST 0x81		 ///< See datasheet
#define SSD1306_CHARGEPUMP 0x8D			 ///< See datasheet
#define SSD1306_SEGREMAP 0xA0			 ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON 0xA5		 ///< Not currently used
#define SSD1306_NORMALDISPLAY 0xA6		 ///< See datasheet
#define SSD1306_INVERTDISPLAY 0xA7		 ///< See datasheet
#define SSD1306_SETMULTIPLEX 0xA8		 ///< See datasheet
#define SSD1306_DISPLAYOFF 0xAE			 ///< See datasheet
#define SSD1306_DISPLAYON 0xAF			 ///< See datasheet
#define SSD1306_COMSCANINC 0xC0			 ///< Not currently used
#define SSD1306_COMSCANDEC 0xC8			 ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET 0xD3	 ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5	 ///< See datasheet
#define SSD1306_SETPRECHARGE 0xD9		 ///< See datasheet
#define SSD1306_SETCOMPINS 0xDA			 ///< See datasheet
#define SSD1306_SETVCOMDETECT 0xDB		 ///< See datasheet

#define SSD1306_SETLOWCOLUMN 0x00  ///< Not currently used
#define SSD1306_SETHIGHCOLUMN 0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE 0x40  ///< See datasheet

#define SSD1306_EXTERNALVCC 0x01  ///< External display voltage source
#define SSD1306_SWITCHCAPVCC 0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26			  ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27				  ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A  ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL 0x2E					  ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL 0x2F					  ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3			  ///< Set scroll range

#define SSD1306_SWITCHCAPVCC 0x02 ///< Gen. display voltage from 3.3V

void SD1306::send_cmd(uint8_t c)
{
	i2c.Write(COMMAND, &c, 1);
}
void SD1306::send_cmdList(const uint8_t *b, int sz)
{
	i2c.Write(COMMAND, b, sz);
}

void SD1306::setup()
{
	// Init sequence
	static const uint8_t PROGMEM init1[] = {SSD1306_DISPLAYOFF,			// 0xAE
											SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
											0x80,						// the suggested ratio 0x80
											SSD1306_SETMULTIPLEX};		// 0xA8
	send_cmdList(init1, sizeof(init1));
	send_cmd(_HEIGHT - 1);

	static const uint8_t PROGMEM init2[] = {SSD1306_SETDISPLAYOFFSET,	// 0xD3
											0x0,						// no offset
											SSD1306_SETSTARTLINE | 0x0, // line #0
											SSD1306_CHARGEPUMP,			// 0x8D
											0x14};
	send_cmdList(init2, sizeof(init2));

	auto vccstate = SSD1306_SWITCHCAPVCC;
	//	send_cmd((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

	static const uint8_t PROGMEM init3[] = {SSD1306_MEMORYMODE, // 0x20
											0x00,				// 0x0 act like ks0108
											SSD1306_SEGREMAP | 0x1,
											SSD1306_COMSCANDEC};
	send_cmdList(init3, sizeof(init3));

	uint8_t comPins = 0x02;
	auto contrast = 0x8F;

	if ((_WIDTH == 128) && (_HEIGHT == 32))
	{
		comPins = 0x02;
		contrast = 0x8F;
	}
	else if ((_WIDTH == 128) && (_HEIGHT == 64))
	{
		comPins = 0x12;
		contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF;
	}
	else if ((_WIDTH == 96) && (_HEIGHT == 16))
	{
		comPins = 0x2; // ada x12
		contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0xAF;
	}
	else
	{
		// Other screen varieties -- TBD
	}

	send_cmd(SSD1306_SETCOMPINS);
	send_cmd(comPins);
	send_cmd(SSD1306_SETCONTRAST);
	send_cmd(contrast);

	send_cmd(SSD1306_SETPRECHARGE); // 0xd9
	send_cmd((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
	static const uint8_t PROGMEM init5[] = {
		SSD1306_SETVCOMDETECT, // 0xDB
		0x40,
		SSD1306_DISPLAYALLON_RESUME, // 0xA4
		SSD1306_NORMALDISPLAY,		 // 0xA6
		SSD1306_DEACTIVATE_SCROLL,
		SSD1306_DISPLAYON}; // Main screen turn on
	send_cmdList(init5, sizeof(init5));
}

/**
  * Set Memory Addressing Mode (20h)
  * 2 byte
  * A[1:0] = 00b, Horizontal Addressing Mode
  * A[1:0] = 01b, Vertical Addressing Mode
  * A[1:0] = 10b, Page Addressing Mode (default after RESET)
  * A[1:0] = 11b, Invalid
  * @param mode -- select Mode
  */
void SD1306::setMemoryAddressingMode_20(SSD1306_AddressingMode mode)
{
	// send initial command to the device
	send_data(COMMAND, 0x20);
	send_data(COMMAND, uint8_t(mode));
}

/** Set Column Address [Space] (21h)
  *
  *  3 byte
  *  Command specifies column start address and end address of the display data RAM. This
  *  command also sets the column address pointer to column start address. This pointer is used to define the
  *  current read/write column address in graphic display data RAM.
  *
  *  It setup column start and end address
  *      A[6:0] : Column start address, range : 0-127d, (RESET=0d)
  *      B[6:0]: Column end address, range : 0-127d, (RESET =127d)
  *
  * @param lower  -- up to 127
  * @param higher -- up to 127
  *
  * Note: This command is only for horizontal or vertical addressing mode!
  */
void SD1306::setColumnAddressScope_21(uint8_t lower, uint8_t upper)
{
	send_data(COMMAND, 0x21);
	send_data(COMMAND, lower);
	send_data(COMMAND, upper);
}

/** Set Page Address (22h)
  *
  *  This triple byte command specifies page start address and end address of the display data RAM. This
  *  command also sets the page address pointer to page start address. This pointer is used to define the current
  *  read/write page address in graphic display data RAM. If vertical address increment mode is enabled by
  *  command 20h, after finishing read/write one page data, it is incremented automatically to the next page
  *  address. Whenever the page address pointer finishes accessing the end page address, it is reset back to start
  *  page address.
  *
  *  Setup page start and end address
  *      A[2:0] : Page start Address, range : 0-7d, (RESET = 0d)
  *      B[2:0] : Page end Address, range : 0-7d, (RESET = 7d)
  *
  *  @param lower  -- from 0 up to 7
  *  @param higher -- from 0 up to 7
  *
  *  Note: This command is only for horizontal or vertical addressing mode.
  */
void SD1306::setPageAddressScope_22(uint8_t lower, uint8_t upper)
{
	send_data(COMMAND, 0x22);
	send_data(COMMAND, lower);
	send_data(COMMAND, upper);
}

/** Set Display Start Line (40h~7Fh)
 *  This command sets the Display Start Line register to determine starting address of display RAM, by selecting
 *  a value from 0 to 63. With value equal to 0, RAM row 0 is mapped to COM0. With value equal to 1, RAM
 *  row 1 is mapped to COM0 and so on.
 *  @param startLine -- from 0 to 63
 */
void SD1306::setDisplayStartLine_40_7F(uint8_t startLine)
{
	send_data(COMMAND, (uint8_t)(0x40 | (startLine & 0b00111111)));
}

/** Set Contrast Control for BANK0 (81h)
 *
 * This command sets the Contrast Setting of the display. The chip has 256 contrast steps from 00h to FFh. The
 * segment output current increwhile (_IF_TxE(I2C_OLED));ases as the contrast step value increases.
 * @param value from 0 to 255
 */
void SD1306::setContrast_81(uint8_t value)
{
	send_data(COMMAND, 0x81);
	send_data(COMMAND, value);
}

/** Set Page Start Address For Page Addressing Mode (0xB0-0xB7) command
 *  According the documentation Page MUST be from 0 to 7
 *  @param pageNum -- from 0 to 7
 */
void SD1306::setPageStartAddressForPageAddressingMode_B0_B7(uint8_t pageNum)
{
	send_data(COMMAND, (uint8_t)(0xb0 | (pageNum & 0b00000111)));
}

/** Set Pre-charge Period (D9h)
 *
 * This command is used to set the duration of the pre-charge period. The interval is counted in number of
 * DCLK, where RESET equals 2 DCLKs.
 *
 * Note:
 * @param value -- experimental typical value is 0xF1
 */
void SD1306::setPrecharge_D9(uint8_t value)
{
	send_data(COMMAND, 0xd9);
	send_data(COMMAND, value);
}

/**
 * Entire Display ON (A4h/A5h)
 * A4h command enable display outputs according to the GDDRAM contents.
 * If A5h command is issued, then by using A4h command, the display will resume to the GDDRAM contents.
 * In other words, A4h command resumes the display from entire display “ON” stage.
 * A5h command forces the entire display to be “ON”, regardless of the contents of the display data RAM.
 *
 * @param resume -- if it will be true, then DISPLAY will go ON and redraw content from RAM
 */
void SD1306::setDisplayOn_A4_A5(bool resume)
{
	uint8_t cmd = (uint8_t)(resume ? 0xA4 : 0xA5);
	send_data(COMMAND, cmd);
}

/** Set Normal/Inverse Display (A6h/A7h)
 *
 *  This command sets the display to be either normal or inverse. In normal display a RAM data of 1 indicates an
 *  “ON” pixel while in inverse display a RAM data of 0 indicates an “ON” pixel.
 *  @param inverse -- if true display will be inverted
 */
void SD1306::setInverse_A6_A7(bool inverse)
{
	uint8_t cmd = (uint8_t)(inverse ? 0xA7 : 0xA6);
	send_data(COMMAND, cmd);
}

/** Set Display ON/OFF (AEh/AFh)
 *
 * These single byte commands are used to turn the OLED panel display ON or OFF.
 * When the display is ON, the selected circuits by Set Master Configuration command will be turned ON.
 * When the display is OFF, those circuits will be turned OFF and the segment and common output are in V SS
 * tate and high impedance state, respectively. These commands set the display to one of the two states:
 *  AEh : Display OFF
 *  AFh : Display ON
 */
void SD1306::switchOLEDOn_AE_AF(bool goOn)
{
	if (goOn)
		send_data(COMMAND, 0xAF);
	else
		send_data(COMMAND, 0xAE);
}
/** Charge Pump Capacitor (8D)
 *
 *  The internal regulator circuit in SSD1306 accompanying only 2 external capacitors can generate a
 *  7.5V voltage supply, V CC, from a low voltage supply input, V BAT . The V CC is the voltage supply to the
 *  OLED driver block. This is a switching capacitor regulator circuit, designed for handheld applications.
 *  This regulator can be turned on/off by software command setting.
 *
 * @param goOn -- if true OLED will going to ON
 * @param enableChargePump -- if On Charge Pump WILL be on when Display ON
 *
 * Note: There are two state in the device: NormalMode <-> SleepMode. If device is in SleepMode then the OLED panel power consumption
 * is close to zero.
 */
void SD1306::chargePump_8D(bool chargeOn)
{
	send_data(COMMAND, 0x8D);
	if (chargeOn)
		send_data(COMMAND, 0x14);
	else
		send_data(COMMAND, 0x10);
}

/** Set Display Offset (D3h)
 * The command specifies the mapping of the display start line to one of
 * COM0~COM63 (assuming that COM0 is the display start line then the display start line register is equal to 0).
 * @param verticalShift -- from 0 to 63
 */
void SD1306::setDisplayOffset_D3(uint8_t verticalShift)
{
	send_data(COMMAND, 0xd3);
	send_data(COMMAND, verticalShift);
}

/** Set VcomH Deselect Level (DBh)
 * This is a special command to adjust of Vcom regulator output.
 */
void SD1306::adjustVcomDeselectLevel_DB(uint8_t value)
{
	send_data(COMMAND, 0xdb);
	send_data(COMMAND, value);
}

/** Set Display Clock Divide Ratio/ Oscillator Frequency (D5h)
 *  This command consists of two functions:
 *  1. Display Clock Divide Ratio (D)(A[3:0])
 *      Set the divide ratio to generate DCLK (Display Clock) from CLK. The divide ratio is from 1 to 16,
 *      with reset value = 1. Please refer to section 8.3 (datasheet ssd1306) for the details relationship of DCLK and CLK.
 *  2. Oscillator Frequency (A[7:4])
 *      Program the oscillator frequency Fosc that is the source of CLK if CLS pin is pulled high. The 4-bit
 *      value results in 16 different frequency settings available as shown below. The default setting is 0b1000
 *
 * WARNING: you should NOT call this function with another parameters if you don't know why you do it
 *
 * @param value -- default value is 0x80
 */
void SD1306::setOscillatorFrequency_D5(uint8_t value)
{
	send_data(COMMAND, 0xd5);
	send_data(COMMAND, value);
}

void SD1306::setMultiplexRatio_A8(uint8_t ratio)
{
	send_data(COMMAND, 0xa8);
	send_data(COMMAND, ratio);
}

void SD1306::setCOMPinsHardwareConfiguration_DA(uint8_t val)
{
	send_data(COMMAND, 0xda);
	send_data(COMMAND, 0b00110010 & val);
}

/**
 * Set the page start address of the target display location by command B0h to B7h
 * @param page -- from 0 to 7
 *
 * NOTE: It command is fit ONLY for Page mode
 */
void SD1306::setPage_B0_B7(uint8_t page)
{
	send_data(COMMAND, (uint8_t)(0xb0 | (0b00000111 & page)));
}

/**
 * Set the lower and the upper column.
 * See note from datasheet:
 *
 * In page addressing mode, after the display RAM is read/written, the column address pointer is increased
 * automatically by 1. If the column address pointer reaches column end address, the column address pointer is
 * reset to column start address and page address pointer is not changed. Users have to set the new page and
 * column addresses in order to access the next page RAM content.
 *
 * In normal display data RAM read or write and page addressing mode, the following steps are required to
 * define the starting RAM access pointer location:
 *  • Set the page start address of the target display location by command B0h to B7h.
 *  • Set the lower start column address of pointer by command 00h~0Fh.
 *  • Set the upper start column address of pointer by command 10h~1Fh.
 * For example, if the page address is set to B2h, lower column address is 03h and upper column address is 10h,
 * then that means the starting column is SEG3 of PAGE2.
 *
 * According that we should send first lower value. Next we send upper value.
 *
 * @param column -- from 0 to 127
 *
 * NOTE: It command is fit ONLY for Page mode
 */
void SD1306::setColumn_00_0F(uint8_t column)
{
	uint8_t cmd = (uint8_t)(0x0f & column);
	send_data(COMMAND, cmd);
	cmd = (uint8_t)(0x10 | (column >> 4));
	send_data(COMMAND, cmd);
}
// -------------------- Graphics methods ---------------------------

/**
 *
 * @param screenRAMClear -- clear or not MCU screenRAM. If not MCU will store buffer and can repaint it.
 */
void SD1306::clear(void)
{
	_mutex_cx = _mutex_cy = 0;
	for (uint16_t i = 0; i < _screenBufferLength; i++)
		_screenRAM[i] = 0;
}

/**
 * Send (and display if OLED is ON) RAM buffer to device
 */
void SD1306::refresh(void)
{
	// if (this->_useTask)
	// 	xSemaphoreGive(this->_refreshSem);
	// else
	//{
		setMemoryAddressingMode_20(SSD1306_AddressingMode::Horizontal); // fa start/send*/stop
		setColumnAddressScope_21(0, _WIDTH - 1);
		setPageAddressScope_22(0, _HEIGHT / 8 - 1);
		i2c.Write(DATAONLY, _screenRAM, _screenBufferLength);
	//}
}

/*
void SD1306::S_Task(void *pv)
{
	((SD1306 *)pv)->Task();
}
void SD1306::Task()
{
	for (;;)
	{
		// aspetto il comando per disegnare lo schermo
		xSemaphoreTake(this->_refreshSem, portMAX_DELAY);

		setMemoryAddressingMode_20(SSD1306_AddressingMode::Horizontal); // fa start/send/stop
		setColumnAddressScope_21(0, _WIDTH - 1);
		setPageAddressScope_22(0, _HEIGHT / 8 - 1);
		i2c.Write(DATAONLY, _screenRAM, _screenBufferLength);
	}
}
*/
///////////////////////////////////////////////

void SD1306::drawVPattern(uint8_t x, int8_t y, uint8_t pattern)
{
	if (y > _HEIGHT || y < (-7) || x > _WIDTH)
		return;
	uint8_t yy = abs(y) % 8;
	if (y < 0)
		_screenRAM[y / 8 * _WIDTH + x] |= pattern >> yy;
	else if (y > 23)
		_screenRAM[y / 8 * _WIDTH + x] |= pattern << yy;
	else
	{
		if (yy != 0)
		{
			_screenRAM[(y / 8 + 0) * _WIDTH + x] |= pattern << yy;
			_screenRAM[(y / 8 + 1) * _WIDTH + x] |= pattern >> (8 - yy);
		}
		else
			_screenRAM[y / 8 * _WIDTH + x] |= pattern;
	}
}

void SD1306::drawPixel(int x, int y, SSD1306_COLOR color)
{
	if (x >= 0 && x < this->_WIDTH && y >= 0 && y < this->_HEIGHT)
	{
		switch (color)
		{
		case SSD1306_COLOR::white:
			this->_screenRAM[x + (y / 8) * this->_WIDTH] |= (1 << (y & 7));
			break;
		case SSD1306_COLOR::black:
			this->_screenRAM[x + (y / 8) * this->_WIDTH] &= ~(1 << (y & 7));
			break;
		case SSD1306_COLOR::inverse:
			this->_screenRAM[x + (y / 8) * this->_WIDTH] ^= (1 << (y & 7));
			break;
		}
	}
}
SSD1306_COLOR SD1306::getPixel(int x, int y)
{
	auto v = this->_screenRAM[x + (y / 8) * this->_WIDTH] & (1 << (y & 7));
	if (v)
		return SSD1306_COLOR::white;
	else
		return SSD1306_COLOR::black;
}

void SD1306::drawHLine(int x1, int x2, int y, SSD1306_COLOR color)
{
	switch (color)
	{
	case SSD1306_COLOR::white:
		for (int x = x1; x < x2; ++x)
			this->_screenRAM[x + (y / 8) * this->_WIDTH] |= (1 << (y & 7));
		break;
	case SSD1306_COLOR::black:
		for (int x = x1; x < x2; ++x)
			this->_screenRAM[x + (y / 8) * this->_WIDTH] &= ~(1 << (y & 7));
		break;
	case SSD1306_COLOR::inverse:
		for (int x = x1; x < x2; ++x)
			this->_screenRAM[x + (y / 8) * this->_WIDTH] ^= (1 << (y & 7));
		break;
	}
}

void SD1306::drawVLine(int x, int y1, int y2, SSD1306_COLOR color)
{
	switch (color)
	{
	case SSD1306_COLOR::white:
		for (int y = y1; y < y2; ++y)
			this->_screenRAM[x + (y / 8) * this->_WIDTH] |= (1 << (y & 7));
		break;
	case SSD1306_COLOR::black:
		for (int y = y1; y < y2; ++y)
			this->_screenRAM[x + (y / 8) * this->_WIDTH] &= ~(1 << (y & 7));
		break;
	case SSD1306_COLOR::inverse:
		for (int y = y1; y < y2; ++y)
			this->_screenRAM[x + (y / 8) * this->_WIDTH] ^= (1 << (y & 7));
		break;
	}
}

void SD1306::scroll()
{
	int h = hch(_mutex_font);

	if (false)
	{
		for (int y = h; y < this->_HEIGHT; y++)
			for (int x = 0; x < this->_WIDTH / 8; ++x)
				this->_screenRAM[x + ((y - h) / 8) * this->_WIDTH] = this->_screenRAM[x + ((y - 0) / 8) * this->_WIDTH];
		for (int y = _mutex_cy; y < this->_HEIGHT; ++y)
			drawHLine(0, this->_WIDTH, y, SSD1306_COLOR::black);
	}
	else
	{
		for (int y = h; y < this->_HEIGHT; y++)
			for (int x = 0; x < this->_WIDTH; ++x)
				drawPixel(x, y - h, getPixel(x, y));

		for (int y = _mutex_cy; y < this->_HEIGHT; ++y)
			drawHLine(0, this->_WIDTH, y, SSD1306_COLOR::black);
	}
}

static inline int s_abs(int s)
{
	return s >= 0 ? s : -s;
}

void SD1306::drawLine(int x0, int y0, int x1, int y1, SSD1306_COLOR color)
{
	int dx = s_abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = s_abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;
	for (;;)
	{
		this->drawPixel(x0, y0, color);
		if (x0 == x1 && y0 == y1)
			break;
		e2 = err;
		if (e2 > -dx)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy)
		{
			err += dx;
			y0 += sy;
		}
	}
}

void SD1306::drawCircle(int x0, int y0, int radius, SSD1306_COLOR color)
{
	int f = 1 - radius;
	int ddF_x = 0;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	drawPixel(x0, y0 + radius, color);
	drawPixel(x0, y0 - radius, color);
	drawPixel(x0 + radius, y0, color);
	drawPixel(x0 - radius, y0, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;
		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 + x, y0 - y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + y, y0 + x, color);
		drawPixel(x0 - y, y0 + x, color);
		drawPixel(x0 + y, y0 - x, color);
		drawPixel(x0 - y, y0 - x, color);
	}
}

void SD1306::drawString(const char *s, SSD1306_COLOR color)
{
	const int w = wch(_mutex_font);
	const int h = hch(_mutex_font);
	while (*s)
	{
		if (_mutex_cx > this->_WIDTH - w)
		{
			_mutex_cy += h;
			_mutex_cx = 0;
		}
		if (_mutex_cy > this->_HEIGHT - h)
		{
			_mutex_cy -= h;
			_mutex_cx = 0;
			scroll();
		}

		if (*s >= 32 && *s <= 126)
		{
			for (int y = 0; y < h; y++)
			{
				auto v = pch2(_mutex_font, *s, y);
				uint32_t bb = 1u << (8 * sizeof(v) - 1);
				for (int x = 0; x < w; ++x)
				{
					auto c = (v & bb) ? SSD1306_COLOR::white : SSD1306_COLOR::black;
					if (color == SSD1306_COLOR::black)
						c = c == SSD1306_COLOR::white ? SSD1306_COLOR::black : SSD1306_COLOR::white;
					drawPixel(x + _mutex_cx, y + _mutex_cy, c);
					bb >>= 1;
				}
			}
		}
		else if ((s[0] == '\n' && s[1] == '\r') || (s[0] == '\r' && s[1] == '\n'))
		{
			++s;
			_mutex_cx = -w;
			_mutex_cy += h;
		}
		else if (*s == '\n' || *s == '\r')
		{
			_mutex_cx = -w;
			_mutex_cy += h;
		}

		++s;
		_mutex_cx += w;
	}
}
