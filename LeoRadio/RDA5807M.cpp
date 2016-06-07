#include "I2C.h"
#include "RDA5807M.h"


uint8_t RDA5807M::_rc = 0;

bool RDA5807M::begin(band_t band) 
{
	_rc = 0;
	I2c.pullup(false);
	I2c.setSpeed(false);
	I2c.timeOut(200);
	I2c.begin();

	setRegister(RDA5807M_R02_CONFIG, 
			RDA5807M_R02_DHIZ | RDA5807M_R02_DMUTE | RDA5807M_R02_BASS | RDA5807M_R02_SEEKUP | RDA5807M_R02_RDS_EN | 
			/*RDA5807M_R02_NEW_METHOD |*/ RDA5807M_R02_ENABLE);
	if (_rc) return false;

	//updateRegister(RDA5807M_R03_TUNING, RDA5807M_R03_BAND_MASK, band << RDA5807M_R03_BAND_SHIFT);
	RDA5807M::band(band);
	if (_rc) return false;

	uint16_t reg = getRegister(RDA5807M_R00_CHIPID);
	if (_rc) return false;
	return (reg >> 8) == RDA5807M_CHIPID;
}

void RDA5807M::dumpRegister(uint8_t reg)
{
	Serial.print(F("R0"));
	Serial.print(reg, HEX);
	Serial.print(F("="));
	uint16_t v = getRegister(reg);
	if (_rc) { Serial.print(F("ERR")); Serial.println(_rc); }
	else
		Serial.println(v, HEX);
}
void RDA5807M::dumpRegisters()
{
	for (uint8_t r = 0; r <= 0x0F; ++r)
		dumpRegister(r);
}
bool RDA5807M::dumpBusScan()
{
	bool ret = false;
	Serial.println(F("Start i2c scan bus...."));
	I2c.scan();
	return ret;
}

void RDA5807M::end() 
{
	setRegister(RDA5807M_R02_CONFIG, 0x00);
}

void RDA5807M::setRegister(uint8_t reg, const uint16_t value) 
{
	if (_rc) return;

	uint8_t t = 10;
	do 
	{
		uint8_t b[2];
		b[0] = highByte(value);
		b[1] = lowByte(value);
		_rc = I2c.write(RDA5807M_I2C_ADDR_RANDOM, reg, b, 2);
		if (_rc == 0) return;
		delay(5);
	}
	while (--t > 0);
	if (_rc) { Serial.print(F("ERR")); Serial.print(_rc); Serial.print(F("  ")); Serial.print(__LINE__); Serial.print("#"); delay(5000); }
}

uint16_t RDA5807M::getRegister(uint8_t reg)
{
	uint8_t t = 10;
	if (_rc) return 0xffff;

	do 
	{
		_rc = I2c.read(uint8_t(RDA5807M_I2C_ADDR_RANDOM), reg, uint8_t(2));
		if (_rc == 0) 
		{
			uint16_t r = 0;
			r |= uint16_t(I2c.receive()) << 8;
			r |= uint16_t(I2c.receive()) << 0;
			return r;
		}
		delay(5);
	}
	while (--t > 0);
	if (_rc) { Serial.print(F("ERR")); Serial.print(_rc); Serial.print(F("  ")); Serial.print(__LINE__); Serial.print("#REG="); Serial.print(reg); Serial.print("   ");delay(5000); }
	if (_rc) return 0xffff;
}

void RDA5807M::updateRegister(uint8_t reg, uint16_t mask, uint16_t value) {
	if (_rc) return;
	uint16_t v = getRegister(reg);
	if (_rc) return;
	setRegister(reg, v & ~mask | value);
}

/////////////////////////////////////////////////////////////////////////////////////////

uint8_t RDA5807M::volume() { return (getRegister(RDA5807M_R05_VOLUME) & RDA5807M_R05_VOLUME_MASK) >> RDA5807M_R05_VOLUME_SHIFT; }
void RDA5807M::setVolume(uint8_t vol) { updateRegister(RDA5807M_R05_VOLUME, RDA5807M_R05_VOLUME_MASK, vol << RDA5807M_R05_VOLUME_SHIFT); }

bool RDA5807M::volumeUp() 
{
	auto value = volume();
	if (value == RDA5807M_R05_VOLUME_MASK) return false;
	setVolume(value+1);
	return true;
}
bool RDA5807M::volumeDown(bool alsoMute) 
{
	auto value = volume();
	if (value > 0) {
		setVolume(value-1);
		if (value == 1 && alsoMute) mute();
		return true;
	}
	return false;
}

// 0=Mute 1=Normal operation
bool RDA5807M::mute() { return (getRegister(RDA5807M_R02_CONFIG) & RDA5807M_R02_DMUTE) == 0; }
void RDA5807M::setMute(bool value, bool minVolume) 
{ 
	updateRegister(RDA5807M_R02_CONFIG, RDA5807M_R02_DMUTE, value ? 0 : RDA5807M_R02_DMUTE); 
	if (!value && minVolume)
		setVolume(1);
}

// 0=Stereo 1=Force mono
bool RDA5807M::mono() { return (getRegister(RDA5807M_R02_CONFIG) & RDA5807M_R02_MONO) != 0; }
void RDA5807M::setMono(bool value)
{ 
	updateRegister(RDA5807M_R02_CONFIG, RDA5807M_R02_MONO, value ? RDA5807M_R02_MONO : 0); 
}
// 0=Disabled 1=Bass boost enabled
bool RDA5807M::bassBoost() { return (getRegister(RDA5807M_R02_CONFIG) & RDA5807M_R02_BASS) != 0; }
void RDA5807M::setBassBoost(bool value)
{ 
	updateRegister(RDA5807M_R02_CONFIG, RDA5807M_R02_BASS, value ? RDA5807M_R02_BASS : 0); 
}

//////////////////////////////////////////////////////////////

// 0=wrap scan 1=stop seek at lower/higher limit 
bool RDA5807M::seekWrap() { return (getRegister(RDA5807M_R02_CONFIG) & RDA5807M_R02_SKMODE) != 0; }
void RDA5807M::seekWrap(bool b) {
	updateRegister(RDA5807M_R02_CONFIG, RDA5807M_R02_SKMODE, b ? 0 : RDA5807M_R02_SKMODE);
}

void RDA5807M::seekUp(bool singleStep) 
{
	updateRegister(RDA5807M_R02_CONFIG,
			(RDA5807M_R02_SEEKUP | RDA5807M_R02_SEEK), 
			(RDA5807M_R02_SEEKUP | RDA5807M_R02_SEEK));

	if (singleStep)
		updateRegister(RDA5807M_R02_CONFIG, RDA5807M_R02_SEEK, 0);
}
void RDA5807M::seekDown(bool singleStep) 
{
	updateRegister(RDA5807M_R02_CONFIG, (RDA5807M_R02_SEEKUP | RDA5807M_R02_SEEK), RDA5807M_R02_SEEK);

	if (singleStep)
		updateRegister(RDA5807M_R02_CONFIG, RDA5807M_R02_SEEK, 0);
}

const PROGMEM uint16_t RDA5807M_BandLowerLimits[5] = { 8700, 7600, 7600, 6500, 5000 };
const PROGMEM uint8_t  RDA5807M_ChannelSpacings[4] = { 100, 200, 50, 25 };

uint16_t RDA5807M::frequency() 
{
	auto band_space = getRegister(RDA5807M_R03_TUNING) & (RDA5807M_R03_BAND_MASK | RDA5807M_R03_SPACE_MASK);
	auto space = (band_space & RDA5807M_R03_SPACE_MASK) >> RDA5807M_R03_SPACE_SHIFT;
	auto band  = (band_space & RDA5807M_R03_BAND_MASK)  >> RDA5807M_R03_BAND_SHIFT;

	if (band == 0b11 && !(getRegister(RDA5807M_R07_BLEND) & RDA5807M_R07_65M_50M_MODE))
		band += 1; //Lower band limit is 50MHz

	uint16_t channel = (getRegister(RDA5807M_R0A_STATUS) & RDA5807M_R0A_READCHAN_MASK) >> RDA5807M_R0A_READCHAN_SHIFT;
	return (uint16_t)pgm_read_word(&RDA5807M_BandLowerLimits[band]) + channel *
		pgm_read_byte(&RDA5807M_ChannelSpacings[space]) / 10;
}

void RDA5807M::setFrequency(uint16_t freq)
{
	auto band_space = getRegister(RDA5807M_R03_TUNING) & (RDA5807M_R03_BAND_MASK | RDA5807M_R03_SPACE_MASK);
	auto space = (band_space & RDA5807M_R03_SPACE_MASK) >> RDA5807M_R03_SPACE_SHIFT;
	auto band  = (band_space & RDA5807M_R03_BAND_MASK)  >> RDA5807M_R03_BAND_SHIFT;

	if (band == 0b11 && !(getRegister(RDA5807M_R07_BLEND) & RDA5807M_R07_65M_50M_MODE))
		band += 1; //Lower band limit is 50MHz

	// f = band + channel * space / 10
	// channel = (f - band) * 10 / space
	uint16_t channel = (freq - pgm_read_word(&RDA5807M_BandLowerLimits[band])) * 10 /
		pgm_read_byte(&RDA5807M_ChannelSpacings[space]);

	updateRegister(RDA5807M_R03_TUNING, RDA5807M_R03_CHAN_MASK, channel << RDA5807M_R03_CHAN_SHIFT);
}

void RDA5807M::formatFrequency(char *s, uint8_t sz)
{
	auto freq = frequency();
	*s = 0;
	sprintf(s, "%d.%02d", freq/100, freq%100);
}

void RDA5807M::band(band_t b) {
	uint16_t mode_65_50 = RDA5807M_R07_65M_50M_MODE;
	if (b == B11_50_76)
	{
		mode_65_50 = 0;
		b = B11_65_76;
	}
	updateRegister(RDA5807M_R03_TUNING, RDA5807M_R03_BAND_MASK, uint8_t(b) << RDA5807M_R03_BAND_SHIFT);
	updateRegister(RDA5807M_R07_BLEND, RDA5807M_R07_65M_50M_MODE, mode_65_50);
}
RDA5807M::band_t RDA5807M::band() {
	auto band_space = getRegister(RDA5807M_R03_TUNING) & (RDA5807M_R03_BAND_MASK | RDA5807M_R03_SPACE_MASK);
	auto space = (band_space & RDA5807M_R03_SPACE_MASK) >> RDA5807M_R03_SPACE_SHIFT;
	auto band  = (band_space & RDA5807M_R03_BAND_MASK)  >> RDA5807M_R03_BAND_SHIFT;

	if (band == 0b11 && !(getRegister(RDA5807M_R07_BLEND) & RDA5807M_R07_65M_50M_MODE))
		band += 1; //Lower band limit is 50MHz
	return (band_t)band;
}

uint8_t RDA5807M::getRSSI() 
{
	return (getRegister(RDA5807M_R0B_RSSI) & RDA5807M_R0B_RSSI_MASK) >> RDA5807M_R0B_RSSI_SHIFT;
}

uint16_t w16() {
	uint16_t a = 0;
	a |= uint16_t(I2c.receive()) << 8;
	a |= uint16_t(I2c.receive()) << 0;
	return a;
}

void RDA5807M::getRadioInfo(RADIO_INFO &ri)
{
	if (_rc) return;
	uint8_t t = 10;
	do
	{
		_rc = I2c.read(RDA5807M_I2C_ADDR_RANDOM, RDA5807M_R0A_STATUS, (2+4)*2);
		if (_rc == 0)
		{
			ri.R0A = w16();
			ri.R0B = w16();
			for (uint8_t i = 0; i < 4; ++i)
				ri.blk[i] = w16();
			return;
		}
		delay(5);
	}
	while (--t > 0);
	if (_rc) { Serial.print(F("ERR")); Serial.print(_rc); Serial.print(F("  ")); Serial.print(__LINE__); Serial.print("#"); delay(5000); }
}

void RDA5807M::getAudioInfo(AUDIO_INFO &ri)
{
	auto R02 = getRegister(RDA5807M_R02_CONFIG);
	auto R04 = getRegister(RDA5807M_R04_GPIO);

	ri.volume = volume();
	ri.softmute  = (R04 & RDA5807M_R04_SOFTMUTE_EN) != 0;
	ri.bassBoost = (R02 & RDA5807M_R02_BASS) != 0;
	ri.mute      = (R02 & RDA5807M_R02_CONFIG) != 0;
}
