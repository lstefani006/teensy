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

class LoRaClass
{
public:
	LoRaClass();

	int begin(long frequency);
	void end();

	// 0=sta ancora trasmettendo, 1=ok si può trasmettere
	int beginPacket(int implicitHeader = false);

	// async=true ==> aspetta che finisca la trasmissione
	// async=false ==> o si aspetta l'irq o si controlla beginPacket(...) == 1
	void endPacket(bool async = false);

	// se size>0 implicit header, size=0 explicit header
	// ritorna in numero di byte del pacchetto
	int parsePacket(int size = 0);

	int packetRssi();
	float packetSnr();
	long packetFrequencyError();

	size_t write(uint8_t byte);
	size_t write(const uint8_t *buffer, size_t size);

	// numero di byte da leggere
	int available();

	// ritorna >= se c'è da leggere, -1 se non ci sono dati.
	int read();

	// ritorna >= se c'è da leggere, -1 se non ci sono dati.
	int peek();

	void onIrq(void (*callback)());

	// se size>0 implicit header, size=0 explicit header
	void receive(int size = 0);

	void idle();
	void sleep();

	void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
	void setFrequency(long frequency);

	// [6..12] default 7.
	void setSpreadingFactor(int sf);

	// valori 125000, 62500 ecc
	void setSignalBandwidth(long sbw);

	// valori [5..8], default 5
	// i numero tra 5..8 è a denominatore.
	// x=4/CR è proporzionale al numero di bit che portano l'informazione (il resto sono i bit per la correzione di errore) 
	// Rb = SF * BW / 2^SF * 4 / CR ==> Rb = bit rate.
	// Rb = 7 * 125.000 / 2^7 * 4 / 5 ==> 5.5Kbit/sec
	// https://www.loratools.nl/#/airtime
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

	// >= 0 evento di ricezione
	// -1   evento di tx
	// -2   niente da fare
	int HandleIrq();

private:
	void explicitHeaderMode();
	void implicitHeaderMode();

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
	void (*_onIrq)();
};

extern LoRaClass LoRa;

#endif
