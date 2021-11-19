#ifndef VLV_HPP
#define VLV_HPP

#include "ByteStream.h"
#include <vector>

class VLV
{
public:
	VLV();
	VLV(ByteStream &stream);
	~VLV();
	uint32_t getValue();
private:
	// Maximum allowed delta-time is 0x0FFFFFFF - 32 bit - 4 byte value
	uint32_t m_vlv = 0;
	uint32_t m_streamCounter = 0;
};


#endif // VLV_HPP
