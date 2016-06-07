#include "RDSParser.h"

#define DEBUG_FUNC0(fn)          { Serial.print(fn); Serial.println(F("()")); }

RDSParser::RDSParser() 
{
	_sendServiceName = nullptr;
	_sendTime = nullptr;
	_sendText = nullptr;
	_sendDebug = nullptr;

	init();
}

void RDSParser::init() 
{
	_last_textAB = 0;
	_PSEq = 0;
	memset(_PSN, 0, sizeof(_PSN));

	memset(_RDSText, 0, sizeof(_RDSText));
	_lastTextIDX = 0;

	_lastRDSMinutes = 0;

	if (true)
	{
		pi_stat_t *const b = &pi_stat[0];
		pi_stat_t *const e = &pi_stat[pi_stat_sz];
		pi_stat_t *c;

		for (c = b; c != e; c++)
		{
			c->pi = 0;
			c->w = -1;
			c->n = 0;
		}
	}
}

void RDSParser::processData(const uint16_t *block, uint8_t blera, uint8_t blerb)
{
	if (blera == 0b11 || blerb == 0b11) return;
	if (blera || blerb) return;

	uint16_t block1 = block[0];
	uint16_t block2 = block[1];
	uint16_t block3 = block[2];
	uint16_t block4 = block[3];

	if (block1 == 0) 
	{
err:
		init();
		if (_sendServiceName) _sendServiceName(_PSN);
		if (_sendText) _sendText(_RDSText);
		if (_sendDebug) _sendDebug(0, 0, 0, block, pi_stat, pi_stat_sz);
		return;
	}

	uint8_t GT  = (block2 & 0xF000) >> 8;
	if (block2 & 0x800) GT |= 0xB; else GT |= 0xA;

	uint8_t  RB2    = block2 & 0b00000011111;
	uint8_t  PTY    = block2 & 0b01111100000;
	uint8_t  TP     = block2 & 0b10000000000;
	uint16_t pi     = block1;


	// il PI e' ripetuto per alcuni blocchi.....
	// Se lo controllo so se il pacchetto e' affidabile
	// oltre che beccare il prima possibile il PI
	uint8_t score = 1;
	switch (GT) 
	{
	case 0x0B:
	case 0x1B:
	case 0x2B:
	case 0x3B:
	case 0x4B:
	case 0x5B:
	case 0x6B:
	case 0x7B:
	case 0x8B:
	case 0x9B:
	case 0xAB:
	case 0xBB:
	case 0xCB:
	case 0xDB:
		if (!(block1 == block3))
			return;
		score = 5;
		break;

	case 0xEB:
		break;

	case 0xFB:
		// blocco che ripete il PI code, GT, PTY, TA, MS, DI segment, DI address
		if (!(block1 == block3 && block2 == block4))
			return;
		score = 10;
		break;
	}

	uint16_t best_pi = add_pi(pi, score);

	if (_sendDebug)
		_sendDebug(pi, GT, PTY, block, pi_stat, pi_stat_sz);

	if (pi != best_pi)
		return;

	switch (GT) 
	{
	case 0xFB:
		// blocco che ripete il PI code, GT, PTY, TA, MS, DI segment, DI address
		break;

	case 0x0B:
		if (!(block1 == block3)) break;
	case 0x0A:
		{
			// The data received is part of the Service Station Name 
			uint8_t idx = 2 * (block2 & 0b11); // numero compreso tra 0 e 2*3=

			uint8_t c1 = block4 >> 8;
			uint8_t c2 = block4 & 0x00FF;

			if (c1 < 32 || c1 > 126) break;
			if (c2 < 32 || c2 > 126) break;

			if (_PSN[idx] == c1) 
				_PSEq |= _BV(idx);
			else
			{
				_PSEq &= ~(_BV(idx));
				_PSN[idx] = c1;
			}
			idx += 1;

			if (_PSN[idx] == c2) 
				_PSEq |= _BV(idx);
			else
			{
				_PSEq &= ~(_BV(idx));
				_PSN[idx] = c2;
			}
			idx += 1;

			if (idx == 8 && _PSEq == 0b11111111)
			{
				if (_sendServiceName)
					_sendServiceName(_PSN);
			}
		}
		break;

	case 0x1A:
	case 0x1B:
		// Program item number code
		break;

	case 0x2B:
		if (block1 != block3) break;
	case 0x2A:
		{
			// The data received is part of the RDS Text.
			uint8_t idx = block2 & 0b1111;
			if (GT == 0x2A) idx *= 4;
			else            idx *= 2;

			if (idx == 0 && idx < _lastTextIDX)
			{
				if (_sendText)
					_sendText(_RDSText);
			}
			_lastTextIDX = idx;

			uint8_t textAB = block2 & 0b10000;
			if (textAB != _last_textAB) 
			{
				// when this bit is toggled the whole buffer should be cleared.
				_last_textAB = textAB;
				memset(_RDSText, 0, sizeof(_RDSText));
			}

			uint8_t c1;
			uint8_t c2;
			if (GT == 0x2A)
			{
				c1 = block3 >> 8;
				c2 = block3 & 0xFF;
				if (c1 < 32 || c1 > 126) c1 = '#';
				if (c2 < 32 || c2 > 126) c2 = '#';
				_RDSText[idx++] = c1;
				_RDSText[idx++] = c2;
			}
			c1 = block4 >> 8;
			c2 = block4 & 0xFF;
			if (c1 < 32 || c1 > 126) c1 = '#';
			if (c2 < 32 || c2 > 126) c2 = '#';
			_RDSText[idx++] = c1;
			_RDSText[idx++] = c2;
			_RDSText[idx]   = 0;

			for (uint8_t i = 0; i < idx; ++i)
			{
				if (_RDSText[i] == 0xA) _RDSText[i] = ' ';
				if (_RDSText[i] == 0xD) {
					_RDSText[i] = 0;
					if (_sendText)
						_sendText(_RDSText);
					_lastTextIDX = 0;
				}
			}
		}
		break;
	
	case 0x3A:
	case 0x3B:
		// open data application
		break;

	case 0x4A:
		{
			// Clock time and date
			int8_t  off   = (block4 & 0b0000000000011111) >> 0;
			uint8_t sign  = (block4 & 0b0000000000100000) >> 5;
			uint8_t mins  = (block4 & 0b0000111111000000) >> 6;
			uint8_t hour  = (block4 & 0b1111000000000000) >> 12;
			hour |= (block3 & 1) << 4;
			uint32_t mjdc  = (uint32_t(block3) >> 1) | ((uint32_t(block2) & 0b11) << 15);

			if (sign) off = -off;

			if (off >= -12 && off <= 12)
			{
				if (mins < 60 && hour < 24)
				{

					// adjust offset
					//mins += 60 * hour;
					//if (sign)
					//mins -= 30 * off;
					//else
					//mins += 30 * off;

					//Use integer arithmetic at all costs, Arduino lacks an FPU
					uint32_t yp = (mjdc * 10 - 150782) * 10 / 36525;
					uint32_t ys = yp * 36525 / 100;
					uint8_t  mp = (mjdc * 10 - 149561 - ys * 10) * 1000 / 306001;
					uint8_t  k  = (mp == 14 || mp == 15) ? 1 : 0;

					uint8_t  tm_hour = hour;
					uint8_t  tm_tz   = off;
					int8_t   tm_min  = mins;
					uint8_t  tm_mday = mjdc - 14956 - ys - mp * 306001 / 10000;
					uint16_t tm_year = 1900 + yp + k;
					uint8_t  tm_mon  = mp - 1 - k * 12;
					uint8_t  tm_wday = (mjdc + 2) % 7 + 1;

					if (mins != _lastRDSMinutes)
					{
						if (_sendTime)
							_sendTime(mjdc, hour, mins, off);

						_lastRDSMinutes = mins;
					}
				}
			}
		}
		break;

	case 0x4B:
		// open data application
		break;

	case 0x5A:
	case 0x5B:
		// ODA
		break;

	case 0x6A:
	case 0x6B:
		// ODA (2)
		break;

	case 0x7A:
	case 0x7B:
		// ODA (3)
		break;

	case 0xFA:
		{
			uint8_t cx[4];
			cx[0] = block3 >> 8;
			cx[1] = block3 & 0xFF;
			cx[2] = block4 >> 8;
			cx[3] = block4 & 0xFF;

			uint8_t idx_s = (block2 & 0b1) ? 4 : 0;
			uint8_t idx_e = idx_s + 4;
			while (idx_s < idx_e)
			{
				uint8_t c = cx[idx_s & 0b11];
				if (c < 32 || c > 128)
					_PSEq &= ~(_BV(idx_s));
				else
				{
					if (_PSN[idx_s] == c) 
						_PSEq |= _BV(idx_s);
					else
					{
						_PSEq &= ~(_BV(idx_s));
						_PSN[idx_s] = c;
					}
				}
				idx_s += 1;
			}
			if (idx_s == 8 && _PSEq == 0b11111111)
			{
				if (_sendServiceName)
					_sendServiceName(_PSN);
			}
		}
		break;
	}
}

uint16_t RDSParser::add_pi(uint16_t pi, uint8_t score)
{
	uint8_t i = 0;

	pi_stat_t *const b = &pi_stat[0];
	pi_stat_t *const e = &pi_stat[pi_stat_sz];
	pi_stat_t *c;

	int8_t when = -1;
	for (c = b; c != e; c++)
		if (c->w > when)
			when = c->w;
	when += 1;

	for (c = b; c != e; c++)
		if (c->pi == pi)
		{
			c->n += score;
			c->w = when;
			break;
		}
	if (c == e)
	{
		for (c = b; c != e; c++)
			if (c->pi == 0)
			{
				c->pi = pi;
				c->w = when;
				c->n = score;
				break;
			}
		if (c == e) 
		{
			// il vetttore e' pieno, ma e' gia' sortato
			// Devo ammazzare qualcuno...

			// prendo tra quelli con score piu' basso il piu vecchio
			pi_stat_t *w = b + 5;;
			for (c = b + 6; c != e; c++)
				if (c->w > w->w)
					w = c;
			w->pi = pi;
			w->w = when;
			w->n = score;
		}
	}

	// normalizzo lo score
	for (c = b; c != e; c++)
		if (c->n >= 99)
			break;
	if (c != e)
	{
		for (c = b; c != e; c++)
		{
			c->n /= 2;
			if (c->n == 0)
			{
				c->pi = 0;
				c->w = -1;
				c->n = 0;
			}
		}
	}

	// normalizzo when
	for (c = b; c != e; c++)
		if (c->w >= 99)
			break;
	if (c != e) {
		for (c = b; c != e; c++)
			c->w /= 2;
	}

	insertion_sort(pi_stat, pi_stat_sz, [](const pi_stat_t &a, const pi_stat_t &b) { return -cmp(a.n, b.n); });
	return pi_stat[0].pi;
}

