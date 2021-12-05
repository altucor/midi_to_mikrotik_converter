#include "ByteStream.hpp"


ByteStream::ByteStream(std::string &filename)
	: m_stream(filename, std::ios::binary)
{
}

bool ByteStream::isOk()
{
	return m_stream.is_open() && 
		m_stream.good() && 
		!m_stream.eof() && 
		!m_stream.bad();
}

uint64_t ByteStream::tellg()
{
	return m_stream.tellg();
}

uint8_t ByteStream::get8u()
{
	uint8_t val = 0;
	m_stream.read(reinterpret_cast<char*>(&val), sizeof(val));
	return val;
}

uint16_t ByteStream::get16u()
{
	uint16_t val = 0;
	m_stream.read(reinterpret_cast<char*>(&val), sizeof(val));
	return val;
}

uint32_t ByteStream::get32u()
{
	uint32_t val = 0;
	m_stream.read(reinterpret_cast<char*>(&val), sizeof(val));
	return val;
}

std::string ByteStream::get_text_header()
{
	char val[4];
	m_stream.read(val, 4);
	return std::string(val, 4);
}

uint64_t ByteStream::get64u()
{
	uint64_t val = 0;
	m_stream.read(reinterpret_cast<char*>(&val), sizeof(val));
	return val;
}

uint16_t ByteStream::get16u_bswap()
{
	return swap_endian<uint16_t>(get16u());
}
uint32_t ByteStream::get32u_bswap()
{
	return swap_endian<uint32_t>(get32u());
}
uint64_t ByteStream::get64u_bswap()
{
	return swap_endian<uint64_t>(get64u());
}

std::vector<uint8_t> ByteStream::getDataPart(const uint64_t size)
{
	if(size == 0)
		return std::vector<uint8_t>();
	std::vector<uint8_t> data(size);
	m_stream.read(reinterpret_cast<char*>(data.data()), data.size());
	return data;
}
