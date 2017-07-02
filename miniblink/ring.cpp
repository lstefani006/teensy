#include <ring.hpp>

Ring::Ring(uint8_t *buf, int32_t size) 
	: _data(buf), _size(size), _begin(0), _end(0)
{
}

int32_t Ring::WriteCh(uint8_t ch)
{
	if (this->Full()) return -1;

	this->_data[this->_end] = ch;
	this->_end = (this->_end + 1) % this->_size;
	return ch;
}

int32_t Ring::Write(const uint8_t *data, int32_t size)
{
	int32_t i;
	for (i = 0; i < size; i++)
		if (WriteCh(data[i]) < 0)
			return -i;
	return i;
}

int32_t Ring::ReadCh(uint8_t *ch)
{
	if (this->Empty()) return -1;

	uint8_t ret = this->_data[this->_begin];
	this->_begin = (this->_begin + 1) % this->_size;
	if (ch) *ch = ret;
	return ret;
}

int32_t Ring::Read(uint8_t *data, int32_t size)
{
	int32_t i;
	for (i = 0; i < size; i++)
		if (ReadCh(data + i) < 0)
			return i;
	return -i;
}
