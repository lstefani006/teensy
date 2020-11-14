#ifndef SSD1306_LIBRARY_SSD1306_I2C_H
#define SSD1306_LIBRARY_SSD1306_I2C_H

#include <wchar.h>

#include <stdint.h>

#include "i2c_utils.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// address of device is // 011110+SA0+RW - 0x3C or 0x3D
#define DEFAULT_7bit_OLED_SLAVE_ADDRESS 0b0111100

enum class SSD1306_AddressingMode
{
	Horizontal = 0b00,
	Vertical = 0b01,
	Page = 0b10,   // RESET
	INVALID = 0b11 // You MUST NOT USE IT
};

enum class SSD1306_COLOR
{
	white = 0,
	black = 1,
	inverse = 2
};

enum class SSD1306_WRAP
{
	nowrap,
	wrapDisplay,
	wrapCoord
};

class SD1306
{
	ESP32_I2C &i2c;

public:
	void (*Log)(int error);

	SD1306(ESP32_I2C &i2c, int w, int h);

	SD1306(const SD1306 &) = delete;
	SD1306 &operator=(const SD1306 &) = delete;
	//~SD1306();

	void setup();

	void send_cmdList(const uint8_t *, int);
	void send_cmd(uint8_t);

	int w() const { return this->_WIDTH; }
	int h() const { return this->_HEIGHT; }

	// paint commands
	void clear(void);
	void refresh(void);
	void drawPixel(int x, int y, SSD1306_COLOR c = SSD1306_COLOR::white);

	SSD1306_COLOR getPixel(int x, int y);

	void drawHLine(int x1, int x2, int y, SSD1306_COLOR color = SSD1306_COLOR::white);
	void drawVLine(int x, int y1, int y2, SSD1306_COLOR color = SSD1306_COLOR::white);
	void drawLine(int x0, int y0, int x1, int y1, SSD1306_COLOR color = SSD1306_COLOR::white);
	void drawCircle(int x0, int y0, int radius, SSD1306_COLOR color = SSD1306_COLOR::white);

	void drawString(const char *s, SSD1306_COLOR color = SSD1306_COLOR::white);
	void scroll();

	void getCursor(int &cx, int &cy) const
	{
		cx = _mutex_cx;
		cy = _mutex_cy;
	}
	void setCursor(int cx, int cy)
	{
		_mutex_cx = cx;
		_mutex_cy = cy;
	}
	void setFont(const uint8_t &font)
	{
		_mutex_font = &font;
	}

private:
	void drawVPattern(uint8_t x, int8_t y, uint8_t pattern);

	// hardware commands
	void setMemoryAddressingMode_20(SSD1306_AddressingMode mode);
	void setColumnAddressScope_21(uint8_t lower, uint8_t upper);
	void setPageAddressScope_22(uint8_t lower, uint8_t upper);
	void setPageStartAddressForPageAddressingMode_B0_B7(uint8_t pageNum);
	void setDisplayStartLine_40_7F(uint8_t startLine);
	void setContrast_81(uint8_t value);
	void setPrecharge_D9(uint8_t value);
	void setDisplayOn_A4_A5(bool resume); // switch ON/OFF MCU of display
	void setInverse_A6_A7(bool inverse);
	void chargePump_8D(bool chargePump);
	void switchOLEDOn_AE_AF(bool goOn); //switch ON/OFF power switch of the OLED panel
	void setDisplayOffset_D3(uint8_t verticalShift);
	void adjustVcomDeselectLevel_DB(uint8_t value);
	void setOscillatorFrequency_D5(uint8_t value); // you SHOULD use default value (0x80)
	void setMultiplexRatio_A8(uint8_t ratio);
	void setCOMPinsHardwareConfiguration_DA(uint8_t);
	void setPage_B0_B7(uint8_t);
	void setColumn_00_0F(uint8_t);

	void send(uint8_t spec);
	void send_data(uint8_t spec, uint8_t data);
	void start();
	void stop();

	// non è necessario proteggerli da Mutex
	int _WIDTH;
	int _HEIGHT;
	int _screenBufferLength;
	uint8_t *_screenRAM;

	int _mutex_cx;
	int _mutex_cy;
	const uint8_t *_mutex_font;
};

class SD1306_Driver
{
	QueueHandle_t _h;

	static void S_Task(void *pv)
	{
		((SD1306_Driver *)pv)->Task();
	}

public:
	SD1306_Driver(SD1306 &sd) : sd(sd)
	{
		_h = nullptr;
	}
	void setup()
	{
		_h = xQueueCreate(16, sizeof(DrawRoot *));
		xTaskCreate(&S_Task, "SSD1306", 4048, this, 5, nullptr);
	}

	void drawString(const char *s, SSD1306_COLOR color = SSD1306_COLOR::white)
	{
		auto d = new DrawText(s, color);
		xQueueSend(_h, &d, portMAX_DELAY);
	}
	void clear()
	{
		auto d = new DrawClear();
		xQueueSend(_h, &d, portMAX_DELAY);
	}
	void setCursor(int cx, int cy)
	{
		auto d = new DrawSetCursor(cx, cy);
		xQueueSend(_h, &d, portMAX_DELAY);
	}
	void getCursor(int &cx, int &cy)
	{
		auto h = xTaskGetCurrentTaskHandle();
		auto d = new DrawGetCursor(h);
		xQueueSend(_h, &d, portMAX_DELAY);

		xTaskNotifyWait(0, ULONG_MAX, nullptr, portMAX_DELAY);

		cx = d->cx;
		cy = d->cy;
		delete d;
	}
	void setFont(const uint8_t *font)
	{
		auto d = new DrawSetFont(font);
		xQueueSend(_h, &d, portMAX_DELAY);
	}

	void Task()
	{
		bool mustRefresh = true;
		for (;;)
		{
			DrawRoot *d = nullptr;
			if (xQueueReceive(_h, &d, /*portMAX_DELAY*/ 100 / portTICK_RATE_MS) == pdTRUE)
			{
				if (d->Draw(this->sd))
					delete d;
				mustRefresh = true;
			}
			else if (mustRefresh)
			{
				sd.refresh();
				mustRefresh = false;
			}
		}
	}

	struct DrawRoot
	{
		virtual bool Draw(SD1306 &) = 0;
		virtual ~DrawRoot() {}
	};

	class DrawSetFont : public DrawRoot
	{
		const uint8_t *font;

	public:
		DrawSetFont(const uint8_t *font) : font(font) {}
		~DrawSetFont() {}
		virtual bool Draw(SD1306 &sd)
		{
			sd.setFont(*font);
			return true;
		}
	};

	class DrawText : public DrawRoot
	{
		char *s;
		SSD1306_COLOR color;

	public:
		DrawText(const char *s, SSD1306_COLOR color)
		{
			this->s = strdup(s);
			this->color = color;
		}
		~DrawText() { free(this->s); }
		virtual bool Draw(SD1306 &sd)
		{
			sd.drawString(s, color);
			return true;
		}
	};
	struct DrawClear : public DrawRoot
	{
		~DrawClear() {}
		virtual bool Draw(SD1306 &sd)
		{
			sd.clear();
			return true;
		}
	};
	struct DrawSetCursor : public DrawRoot
	{
		const int cx, cy;
		DrawSetCursor(int cx, int cy) : cx(cx), cy(cy) {}
		~DrawSetCursor() {}
		virtual bool Draw(SD1306 &sd)
		{
			sd.setCursor(cx, cy);
			return true;
		}
	};
	struct DrawGetCursor : public DrawRoot
	{
		int cx = -1, cy = -1;
		TaskHandle_t h;
		DrawGetCursor(TaskHandle_t h) : h(h) {}
		~DrawGetCursor() {}
		virtual bool Draw(SD1306 &sd)
		{
			sd.getCursor(cx, cy);
			xTaskNotify(h, 0, eNoAction);
			return false;
		}
	};

private:
	SD1306 &sd;
};

#endif //SSD1306_LIBRARY_SSD1306_I2C_H
