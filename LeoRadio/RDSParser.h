#ifndef __RDSPARSER_H__
#define __RDSPARSER_H__

#include <Arduino.h>

template <typename T> int cmp(const T &a, const T &b) { 
	if (a > b) return +1;
	if (a < b) return -1;
	return 0;
}
template <typename T> void swap(T &a, T &b) { T t(a); a = b; b = t; }
template <typename T, typename CMP> void insertion_sort(T *arr, int sz, CMP cmp) {
	for (int i = 1; i < sz; i++) {
		int j = i;
		while (j > 0 && cmp(arr[j - 1], arr[j]) > 0) {
			swap(arr[j], arr[j - 1]);
			j--;
		}
	}
}

template <typename T> void iter_swap(T a, T b) { swap(*a, *b); }

template<class ForwardIt, class Compare> ForwardIt min_element(ForwardIt first, ForwardIt last, Compare comp)
{
	if (first == last) return last;

	ForwardIt smallest = first;
	++first;
	for (; first != last; ++first)
		if (comp(*first, *smallest))
			smallest = first;
	return smallest;
}

template<class Iterator>
void insertion_sort(Iterator a, Iterator end)
{
	iter_swap(a, min_element(a, end));

	for (Iterator b = a; ++b < end; a = b)
		for(Iterator c = b; *c < *a; --c, --a )
			iter_swap(a, c);
}










struct pi_stat_t
{
	pi_stat_t() { pi = 0; n = 0; w = -1; }
	uint16_t pi;
	uint8_t n;
	int8_t w;
};

/// callback function for passing a ServicenName 
typedef void(*receiveServicenNameFunction)(const char *name);
typedef void(*receiveTextFunction)(const char *name);
typedef void(*receiveTimeFunction)(uint32_t mjdc, uint8_t hour, uint8_t minute, int8_t off);
typedef void(*receiveDebugFunction)(uint16_t pi, uint8_t GT, uint8_t PTY, const uint16_t *b, const pi_stat_t *p, uint8_t p_sz);

class RDSParser
{
public:
	RDSParser();

	void init();

	void processData(const uint16_t *block, uint8_t blera, uint8_t blerb);

	void attachServicenNameCallback(receiveServicenNameFunction f) { _sendServiceName = f; }
	void attachTextCallback(receiveTextFunction f) { _sendText = f; }
	void attachTimeCallback(receiveTimeFunction f) { _sendTime = f; }
	void attachDebugCallback(receiveDebugFunction f) { _sendDebug = f; }

private:
	receiveServicenNameFunction _sendServiceName;
	receiveTimeFunction _sendTime;
	receiveTextFunction _sendText;
	receiveDebugFunction _sendDebug;

	uint8_t _PSEq;
	char _PSN[8 + 1];

	uint16_t _lastRDSMinutes;

	uint8_t _last_textAB, _lastTextIDX;
	char _RDSText[64 + 1];

	uint16_t _lastPI;

	constexpr static int pi_stat_sz = 10;
	pi_stat_t pi_stat[pi_stat_sz];

	uint16_t add_pi(uint16_t pi, uint8_t score);
};

#endif //__RDSPARSER_H__
