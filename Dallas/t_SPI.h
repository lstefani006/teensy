#ifndef t_SPI_h__
#define t_SPI_h__

#include <Arduino.h>
#include <SPI.h>

namespace t
{
	template<uint8_t cs, uint8_t clk, uint8_t mosi, uint8_t miso> class swSPI
	{
		uint8_t bitOrder;
		uint8_t trLevel;

	public:
		class Settings {
		public:
			Settings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) : bitOrder(bitOrder) {}
			uint8_t bitOrder;
		};

		swSPI() { trLevel = 0; bitOrder = MSBFIRST; }

		bool begin() 
		{
			if (cs != 0xff) pinMode(cs, OUTPUT);
			pinMode(clk,  OUTPUT);
			pinMode(mosi, OUTPUT);
			if (miso != 0xff) pinMode(miso, INPUT);
			return true;
		}
		void write(byte data)  
		{
			if (trLevel == 0 && cs != 0xff) digitalWrite(cs, LOW);
			shiftOut(mosi, clk, bitOrder, data);
			if (trLevel == 0 && cs != 0xff) digitalWrite(cs, HIGH);
		}
		byte read() 
		{
			if (trLevel == 0 && cs != 0xff) digitalWrite(cs, LOW);
			byte r = shiftIn(miso, clk, bitOrder);
			if (trLevel == 0 && cs != 0xff) digitalWrite(cs, HIGH);
			return r;
		}

		class SPITransaction {
		public:
			SPITransaction(swSPI &spi, Settings &s) : spi(spi) { spi.beginTransaction(s); }
			~SPITransaction() { spi.endTransaction(); }
		private:
			swSPI &spi;
		};
	private:
		void beginTransaction(Settings &s) {
			if (trLevel == 0) {
				bitOrder = s.bitOrder;
				if (cs != 0xff) digitalWrite(cs, LOW);
			}
			trLevel += 1;
		}
		void endTransaction() {
			trLevel -= 1;
			if (trLevel == 0)
				if (cs != 0xff) digitalWrite(cs, HIGH);
		}
	};

	template<uint8_t cs, uint8_t clk, uint8_t mosi, uint8_t miso> class hwSPI
	{
		uint8_t trLevel;

	public:
		class Settings : public SPISettings {
		public:
			Settings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
				: SPISettings(clock, bitOrder, dataMode) {}
		};

		hwSPI() { trLevel = 0; }

		bool begin() 
		{
			if (SPI.pinIsChipSelect(cs) == false)
				return false;

			if (cs != 0xff) pinMode(cs,   OUTPUT);

			SPI.setMOSI(mosi);
			if (miso != 0xff) SPI.setMISO(miso);
			SPI.setSCK(clk);

			SPI.begin();
			//SPISettings s(4*1000*1000, MSBFIRST, SPI_MODE0);
			//// da chiamare prima di SPI.transfer e di scrivere sul chip select
			//SPI.beginTransaction(s);
			return true;
		}
		byte read()
		{
			if (trLevel == 0 && cs != 0xff) digitalWrite(cs, LOW);
			byte r = SPI.transfer(0xff);
			if (trLevel == 0 && cs != 0xff) digitalWrite(cs, HIGH);
			return r;
		}
		void write(byte data)  
		{
			if (trLevel == 0 && cs != 0xff) digitalWrite(cs, LOW);
			SPI.transfer(data);
			if (trLevel == 0 && cs != 0xff) digitalWrite(cs, HIGH);
		}

		class SPITransaction {
		public:
			SPITransaction(hwSPI &spi, Settings &s) : spi(spi) { spi.beginTransaction(s); }
			~SPITransaction() { spi.endTransaction(); }
		private:
			hwSPI &spi;
		};

	private:
		void beginTransaction(Settings &s) {
			if (trLevel == 0) {
				SPI.beginTransaction(s);
				digitalWrite(cs, LOW);
			}
			trLevel += 1;
		}
		void endTransaction() {
			trLevel -= 1;
			if (trLevel == 0) {
				digitalWrite(cs, HIGH);
				SPI.endTransaction();
			}
		}
	};
}

#endif
