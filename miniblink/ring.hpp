#ifndef __ring_hpp__
#define __ring_hpp__

/******************************************************************************
 *  Simple ringbuffer implementation from open-bldc's libgovernor that
 *  you can find at:
 *  https://github.com/open-bldc/open-bldc/tree/master/source/libgovernor
 ******************************************************************************/

#include <stdint.h>

class Ring
{
public:
	Ring(uint8_t *buf, int32_t size);

	int32_t WriteCh(uint8_t ch);
	int32_t Write(const uint8_t *data, int32_t size);
	int32_t ReadCh(uint8_t *ch = nullptr);
	int32_t Read(uint8_t *data, int32_t size);

	bool Empty() const { return this->_begin == this->_end; }
	bool Full() const { return (this->_end + 1) % this->_size == this->_begin; }
	int32_t Count() const { return _end >= _begin ? _end - _begin : _begin + _size - _end; }

private:
	uint8_t *_data;
	int32_t  _size;
	uint32_t _begin;
	uint32_t _end;
};


#endif
