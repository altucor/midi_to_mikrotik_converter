#ifndef BYTE_STREAM_HPP
#define BYTE_STREAM_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>

template <typename T>
T swap_endian(T u)
{
	static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");
	union
	{
		T u;
		unsigned char u8[sizeof(T)];
	} source, dest;
	source.u = u;
	for (size_t k = 0; k < sizeof(T); k++)
		dest.u8[k] = source.u8[sizeof(T) - k - 1];

	return dest.u;
}

class ByteStream
{
public:
	ByteStream(std::string &filename);
	bool isOk();
	uint64_t tellg();
	uint8_t get8u();
	uint16_t get16u();
	uint32_t get32u();
	std::string get_text_header();
	uint64_t get64u();
	uint16_t get16u_bswap();
	uint32_t get32u_bswap();
	uint64_t get64u_bswap();
	std::vector<uint8_t> getDataPart(const uint64_t size);
private:
	std::ifstream m_stream;
};

#endif // BYTE_STREAM_HPP
