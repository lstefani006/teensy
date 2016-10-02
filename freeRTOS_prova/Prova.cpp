#include <stdlib.h>
#include <string.h>
#include <Arduino.h>

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

#include <uprintf.hpp>

class Semaphore
{
public:
	Semaphore() { _h = nullptr; }
	~Semaphore() { if (_h) vSemaphoreDelete(_h); }

	bool take(int t = portMAX_DELAY) { return xSemaphoreTake(_h, t) == pdPASS; }
	void give() { xSemaphoreGive(_h); }

	struct Lock
	{
		Lock(Semaphore &s,int t) : _s(s) { _b = s.take(t); }
		operator bool () { return _b; }
		~Lock() { if (_b) _s.give(); }
		Semaphore &_s;
		bool _b;
	};

protected:
	SemaphoreHandle_t _h;
};

// E' un semaforo t.c. il task che fa take DEVE fare give per rilasciare 
class SemaphoreMutex : public Semaphore
{
public:
	bool begin() { _h = xSemaphoreCreateMutex(); return _h != nullptr; }
};

// E' un semaforo dove un task fa take e un altro fa give
class SemaphoreBinary : public Semaphore
{
public:
	bool begin() { _h = xSemaphoreCreateBinary(); return _h != nullptr; }
};

class SemaphoreCounting : public Semaphore
{
public:
	bool begin(int maxCount, int initialCount) { _h = xSemaphoreCreateCounting(maxCount, initialCount); return _h != nullptr; }
};

template<class T>
class Queue
{
public:
	Queue() { _h = nullptr; }
	bool create(int qLen) { _h = xQueueCreate(qLen, sizeof(T)); return _h != nullptr; }
	bool SendToBack(const T &v, int t) { return xQueueSendToBack(_h, &v, t) == pdPASS; }
	bool SendToFront(const T &v, int t) { return xQueueSendToFront(_h, &v, t) == pdPASS; }

	bool Receive(T &ret, int t) { return xQueueReceive(_h, &ret, t) == pdPASS; }
	bool Peek(T &ret, int t) { return xQueuePeek(_h, &ret, t) == pdPASS; }

	int Len() { return uxQueueMessagesWaiting(_h); }

private:
	QueueHandle_t _h;
};

class Task
{
public:
	Task() : _h(nullptr) {}

	bool create(void (*f)(void*), const char *taskName, int szStack, void *arg, int priority) {
		return xTaskCreate(f, taskName, szStack, arg, priority, &_h) == pdPASS;
	}
	~Task() { if (_h) vTaskDelete(_h);}

	TickType_t GetTickCount() { return xTaskGetTickCount(); }
	void Delay(int ticks) { vTaskDelay(ticks); }
	void Dalay(TickType_t from, int ticks) { vTaskDelayUntil(&from, ticks); }

private:
	TaskHandle_t _h;
};


void Blink(void *);
void Print(void *);
void Luca(void *);

Task taskLuca;
Task taskPrint;
Task taskBlink;


Queue<char> PrintQueue;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(38400);

	PrintQueue.create(64);
	uprintf_cb = [](char ch) { PrintQueue.SendToBack(ch, portMAX_DELAY); return true; };

	taskBlink.create(Blink, "Blink", 200, nullptr, 1);
	taskLuca.create(Luca,  "Luca", 200, nullptr, 1);
	taskPrint.create(Print, "Print", 200, nullptr, 1);
}

void loop()
{
}

void Print(void *)
{
	for (;;)
	{
		char ch = 0;
		PrintQueue.Receive(ch, portMAX_DELAY);
		Serial.write(ch);
	}
}

void Luca(void *)
{
	int n = 0;
	for (;;)
	{
		uprintf("Luca %d %d\n", n++, portTICK_PERIOD_MS);
		vTaskDelay(10000 / portTICK_PERIOD_MS );

		auto vv = uxTaskGetStackHighWaterMark(nullptr);
		uprintf("Stack mark = %d\n", vv);
	}
}
void Blink(void *)
{
	int n = 0;
	for (;;)
	{
		uprintf("b %d\n", n++);

		digitalWrite(LED_BUILTIN, HIGH); 
		vTaskDelay(500 / portTICK_PERIOD_MS );
		digitalWrite(LED_BUILTIN, LOW);   
		vTaskDelay(500 / portTICK_PERIOD_MS );
	}
}
