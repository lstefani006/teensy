#ifndef t_SPI_h__
#define t_SPI_h__

#include <Arduino.h>
#include <SPI.h>

namespace t
{
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
#if defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISK)
			if (SPI.pinIsChipSelect(cs) == false)
				return false;

			if (cs != 0xff) pinMode(cs,   OUTPUT);

			SPI.setMOSI(mosi);
			if (miso != 0xff) SPI.setMISO(miso);
			SPI.setSCK(clk);
#endif

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
