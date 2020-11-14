#ifndef LORA_H
#define LORA_H

#include <Arduino.h>

//#define LORA_DEFAULT_SPI SPI
#define LORA_DEFAULT_SPI_FREQUENCY 200000
#define LORA_DEFAULT_SS_PIN 10
#define LORA_DEFAULT_RESET_PIN 9
#define LORA_DEFAULT_DIO0_PIN 2

#define PA_OUTPUT_RFO_PIN 0
#define PA_OUTPUT_PA_BOOST_PIN 1

class LoRaClass // : public Stream
{
public:
	LoRaClass();

	int begin(long frequency);
	void end();

	int beginPacket(int implicitHeader = false);
	int endPacket(bool async = false);

	int parsePacket(int size = 0);
	int packetRssi();
	float packetSnr();
	long packetFrequencyError();

	// from Print
	virtual size_t write(uint8_t byte);
	virtual size_t write(const uint8_t *buffer, size_t size);

	// from Stream

	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();


	void onReceive(void (*callback)(int));
	void onTxDone(void (*callback)());

	void receive(int size = 0);

	void idle();
	void sleep();

	void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
	void setFrequency(long frequency);
	void setSpreadingFactor(int sf);
	void setSignalBandwidth(long sbw);
	void setCodingRate4(int denominator);
	void setPreambleLength(long length);
	void setSyncWord(int sw);
	void enableCrc();
	void disableCrc();
	void enableInvertIQ();
	void disableInvertIQ();

	void setOCP(uint8_t mA); // Over Current Protection control

	// deprecated
	void crc() { enableCrc(); }
	void noCrc() { disableCrc(); }

	byte random();

	void setPins(gpio_num_t ss, gpio_num_t reset, gpio_num_t dio0);
	void setSPI(SPIClass &spi);
	void setSPIFrequency(uint32_t frequency);

	void dumpRegisters(Stream &out);

private:
	void explicitHeaderMode();
	void implicitHeaderMode();

	void handleDio0Rise();
	bool isTransmitting();

	int getSpreadingFactor();
	long getSignalBandwidth();

	void setLdoFlag();

	uint8_t readRegister(uint8_t address);
	void writeRegister(uint8_t address, uint8_t value);
	uint8_t singleTransfer(uint8_t address, uint8_t value);

	static void onDio0Rise(void *);

private:
	SPISettings _spiSettings;
	SPIClass *_spi;
	gpio_num_t _ss;
	gpio_num_t _reset;
	gpio_num_t _dio0;
	long _frequency;
	int _packetIndex;
	int _implicitHeaderMode;
	void (*_onReceive)(int);
	void (*_onTxDone)();
	int _irqCounter;
	void manageInterrupt(bool attach);
};

extern LoRaClass LoRa;

#endif