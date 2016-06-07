/* Arduino RDA5807M Library
 * See the README file for author and licensing information. In case it's
 * missing from your distribution, use the one here as the authoritative
 * version: https://github.com/csdexter/RDA5807M/blob/master/README
 *
 * This library is for interfacing with a RDA Microelectronics RDA5807M
 * single-chip FM broadcast radio receiver.
 * See the example sketches to learn how to use the library in your code.
 *
 * This is the main include file for the library.
 */
#ifndef _RDA5807M_H_INCLUDED
#define _RDA5807M_H_INCLUDED

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

//Define RDA5807M I2C Addresses
#define RDA5807M_I2C_ADDR_SEQ    (0x20 >> 1)
#define RDA5807M_I2C_ADDR_RANDOM (0x22 >> 1)
//#define RDA5807M_I2C_ADDR_SEQTEA (0xC0 >> 1)

//Register file origins for sequential mode
#define RDA5807M_FIRST_REGISTER_WRITE  0x02
#define RDA5807M_FIRST_REGISTER_READ   0x0A
#define RDA5807M_LAST_REGISTER         0x3A

//Register addresses
#define RDA5807M_R00_CHIPID  0x00
#define RDA5807M_R02_CONFIG  0x02
#define RDA5807M_R03_TUNING  0x03
#define RDA5807M_R04_GPIO    0x04
#define RDA5807M_R05_VOLUME  0x05
#define RDA5807M_R06_I2S     0x06
#define RDA5807M_R07_BLEND   0x07
#define RDA5807M_R08_FREQ    0x08
#define RDA5807M_R0A_STATUS  0x0A
#define RDA5807M_R0B_RSSI    0x0B
#define RDA5807M_R0C_RDSA    0x0C
#define RDA5807M_R0D_RDSB    0x0D
#define RDA5807M_R0E_RDSC    0x0E
#define RDA5807M_R0F_RDSD    0x0F

#define RDA5807M_CHIPID            0x58

// R02_CONFIG
#define RDA5807M_R02_DHIZ                (0x8000)
#define RDA5807M_R02_DMUTE               (0x4000)
#define RDA5807M_R02_MONO                (0x2000)
#define RDA5807M_R02_BASS                (0x1000)
#define RDA5807M_R02_RCLKNOCAL           (0x0800)
#define RDA5807M_R02_RCLKDIRECT          (0x0400)
#define RDA5807M_R02_SEEKUP              (0x0200)
#define RDA5807M_R02_SEEK                (0x0100)
#define RDA5807M_R02_SKMODE              (0x0080)
#define RDA5807M_R02_CLKMODE_MASK        (0x0070)
#define RDA5807M_R02_CLKMODE_32K         (0x0 << 4)
#define RDA5807M_R02_CLKMODE_12M         (0x1 << 4)
#define RDA5807M_R02_CLKMODE_13M         (0x2 << 4)
#define RDA5807M_R02_CLKMODE_19M         (0x3 << 4)
#define RDA5807M_R02_CLKMODE_24M         (0x5 << 4)
#define RDA5807M_R02_CLKMODE_26M         (0x6 << 4)
#define RDA5807M_R02_CLKMODE_38M         (0x7 << 4)
#define RDA5807M_R02_RDS_EN              (0x0008)
#define RDA5807M_R02_NEW_METHOD          (0x0004)
#define RDA5807M_R02_SOFT_RESET          (0x0002)
#define RDA5807M_R02_ENABLE              (0x0001)
// RDA5807M_REG_TUNING_03
#define RDA5807M_R03_CHAN_MASK           (0xFFC0)
#define RDA5807M_R03_CHAN_SHIFT          (6)
#define RDA5807M_R03_DIRECT_MODE         (0x0020)
#define RDA5807M_R03_TUNE                (0x0010)
#define RDA5807M_R03_BAND_MASK           (0b1100)
#define RDA5807M_R03_BAND_SHIFT          (2)
#define RDA5807M_R03_BAND_87_108         (0x0 << 2)
#define RDA5807M_R03_BAND_76_91          (0x1 << 2)
#define RDA5807M_R03_BAND_76_108         (0x2 << 2)
#define RDA5807M_R03_BAND_65_76__50_65   (0x3 << 2)
#define RDA5807M_R03_SPACE_MASK          (0x0003)
#define RDA5807M_R03_SPACE_SHIFT         (0)
#define RDA5807M_R03_SPACE_100K          (0x0)
#define RDA5807M_R03_SPACE_200K          (0x1)
#define RDA5807M_R03_SPACE_50K           (0x2)
#define RDA5807M_R03_SPACE_25K           (0x3)

// RDA5807M_REG_GPIO_04
#define RDA5807M_R04_DE                  (0x0800)
#define RDA5807M_R04_SOFTMUTE_EN         (0x0200)
#define RDA5807M_R04_AFCD                (0x0100)

// RDA5807M_REG_VOLUME_05
#define RDA5807P_R05_INT_MODE            (0x8000)
#define RDA5807M_R05_SEEKTH_MASK         (0x0F00)
#define RDA5807M_R05_SEEKTH_SHIFT        (8)
#define RDA5807M_R05_VOLUME_MASK         (0x000F)
#define RDA5807M_R05_VOLUME_SHIFT        (0)

// RDA5807M_REG_I2S_06
#define RDA5807M_R06_OPENMODE_MASK       (0x6000)
#define RDA5807M_R06_OPENMODE_WRITE      (0x3 << 13)

// RDA5807M_REG_BLEND_07
#define RDA5807M_R07_SOFRBLENDTH_MASK    (0x7C00)
#define RDA5807M_R07_SOFRBLENDTH_SHIFT   (10)
#define RDA5807M_R07_65M_50M_MODE        (0x0200)
#define RDA5807M_R07_SEEKTH_OLD_MASK     (0x00FC)
#define RDA5807M_R07_SEEKTH_OLD_SHIFT    (2)
#define RDA5807M_R07_SOFTBLEND_EN        (0x0002)
#define RDA5807M_R07_FREQ_MODE           (0x0001)

// RDA5807M_REG_STATUS_0A
#define RDA5807M_R0A_STATUS_RDSR         (0x8000)
#define RDA5807M_R0A_STATUS_STC          (0x4000)
#define RDA5807M_R0A_STATUS_SF           (0x2000)
#define RDA5807M_R0A_STATUS_RDSS         (0x1000)
#define RDA5807M_R0A_STATUS_BLK_E        (0x0800)
#define RDA5807M_R0A_STATUS_ST           (0x0400)
#define RDA5807M_R0A_READCHAN_MASK       (0x03FF)
#define RDA5807M_R0A_READCHAN_SHIFT      (0)

// RDA5807M_REG_RSSI_0B
#define RDA5807M_R0B_RSSI_MASK           (0xFE00)
#define RDA5807M_R0B_RSSI_SHIFT          (9)
#define RDA5807M_R0B_FM_TRUE             (0x0100)
#define RDA5807M_R0B_FM_READY            (0x0080)
#define RDA5807M_R0B_ABCD_E              (0b10000)
#define RDA5807M_R0B_BLERA_MASK          (0b1100)
#define RDA5807M_R0B_BLERA_SHIFT         (2)
#define RDA5807M_R0B_BLERB_MASK          (0b11)
#define RDA5807M_R0B_BLERB_SHIFT         (0)

extern const PROGMEM uint16_t RDA5807M_BandLowerLimits[];
extern const PROGMEM uint8_t  RDA5807M_ChannelSpacings[];

class RDA5807M
{
public:
	static uint8_t error() { return _rc; }

	~RDA5807M() { end(); };

	/*
	 *   Mutes and disables the chip.
	 */
	static void end();

	/*
	 *   Initializes the RDA5807M, starts the radio and configures band
	 *   limits.
	 * Parameters:
	 *   band - The desired band limits, one of the RDA5807M_BAND_* 
	 *          constants.
	 */
	enum band_t { B00_87_108, B01_76_91, B10_76_108, B11_65_76, B11_50_76 };
	static bool begin(band_t band = B00_87_108);

	/*
	 *   Getter and setter for single random access to registers.
	 * Parameters:
	 *   reg   - register to get or set, one of the RDA5807M_REG_* constants.
	 *   value - value to set the given register to.
	 * Returns:
	 *   current value of given register.
	 */
	static void setRegister(uint8_t reg, uint16_t value);
	static uint16_t getRegister(uint8_t reg);

	/*
	 *   Read-before-write setter for single random access to registers.
	 * Parameters:
	 *   reg   - register to update, one of the RDA5807M_REG_* constants.
	 *   mask  - mask of the bits that are to be updated.
	 *   value - value to set the given register and bits to.
	 */
	static void updateRegister(uint8_t reg, uint16_t mask, uint16_t value);

	/*
	 *   Getter and setter for bulk sequential access to registers. Gets
	 *   always start at RDA5807M_FIRST_REGISTER_READ while sets always
	 *   start at RDA5807M_FIRST_REGISTER_WRITE. The RDA5807M register file
	 *   has exactly RDA5807M_LAST_REGISTER uint16_t-sized entries.
	 * Parameters:
	 *   count - how many sequential registers to get/set.
	 *   regs  - will be filled with the values of the got registers or will
	 *           be the source of the values for the set registers.
	 */
	static void setRegisterBulk(uint8_t count, const uint16_t regs[]);
	static void getRegisterBulk(uint8_t count, uint16_t regs[]);

	static uint8_t volume();
	static void    setVolume(uint8_t vol);
	static bool    volumeUp();
	static bool    volumeDown(bool alsoMute = false);

	static bool mute();
	static void setMute(bool value, bool minVolume = false); // se value=false con minVolume=true metto il vol al minimo

	static bool mono();
	static void setMono(bool value);

	static bool bassBoost();
	static void setBassBoost(bool value);

	static bool seekWrap();
	static void seekWrap(bool value);

	static void seekUp(bool singleStep = false);
	static void seekDown(bool singleStep = false);

	static uint16_t frequency();
	static void setFrequency(uint16_t value);
	static void formatFrequency(char *s, uint8_t sz);

	static void band(band_t);
	static band_t band();

	/*
	 *   Retrieves the Received Signal Strength Indication measurement for
	 *   the currently tuned station.
	 */
	static uint8_t getRSSI();

	struct RADIO_INFO 
	{
		uint16_t R0A;
		uint16_t R0B;
		uint16_t blk[4];

#define ra_rds_ready(r)            (((r.R0A) & RDA5807M_R0A_STATUS_RDSR) != 0)
#define ra_seek_tune_complete(r)   (((r.R0A) & RDA5807M_R0A_STATUS_STC) != 0)
#define ra_seek_fail(r)            (((r.R0A) & RDA5807M_R0A_STATUS_SF) != 0)
#define ra_rds_synchronization(r)  (((r.R0A) & RDA5807M_R0A_STATUS_RDSS) != 0)
#define ra_block_e_found(r)        (((r.R0A) & RDA5807M_R0A_STATUS_BLK_E) != 0)
#define ra_stereo(r)               (((r.R0A) & RDA5807M_R0A_STATUS_ST) != 0)
#define ra_read_channel(r)         (((r.R0A) & RDA5807M_R0A_READCHAN_MASK) !=RRDA5807M_R0A_READCHAN_SHIFT)

#define rb_rssi(r)                 (((r.R0B) & RDA5807M_R0B_RSSI_MASK) >> RDA5807M_R0B_RSSI_SHIFT)
#define rb_fm_true(r)              (((r.R0B) & RDA5807M_R0B_FM_TRUE) != 0)
#define rb_fm_ready(r)             (((r.R0B) & RDA5807M_R0B_FM_READY) != 0)
#define rb_abcd_e(r)               (((r.R0B) & RDA5807M_R0B_ABCD_E) != 0)
#define rb_blera(r)                (((r.R0B) & RDA5807M_R0B_BLERA_MASK) >> RDA5807M_R0B_BLERA_SHIFT)
#define rb_blerb(r)                (((r.R0B) & RDA5807M_R0B_BLERB_MASK) >> RDA5807M_R0B_BLERB_SHIFT)
	};

	static void getRadioInfo(RADIO_INFO &);


	struct AUDIO_INFO 
	{   
		uint8_t volume;
		bool mute:1;
		bool softmute:1;
		bool bassBoost:1;
	};  
	static void getAudioInfo(AUDIO_INFO &);

	static bool dumpBusScan();
	static void dumpRegister(uint8_t reg);
	static void dumpRegisters();

	static uint8_t _rc;
};

#endif
