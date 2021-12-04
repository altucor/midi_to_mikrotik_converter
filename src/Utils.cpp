#include "Utils.hpp"

#include <utility>
#include <iomanip>
#include <sstream>

std::string Utils::toHexBuffer(std::vector<uint8_t> &buf)
{
	if(buf.size() == 0)
		return std::string();
	std::stringstream ss;
	for(std::size_t i=0; i<buf.size(); i++)
	{
		ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((int)buf[i]) << " " << std::dec;
	}
	return ss.str();
}

std::string Utils::toHex(uint64_t val, size_t width)
{
	std::stringstream ss;
	ss << "0x" <<
		std::hex << 
		std::uppercase << 
		std::setfill('0') << 
		std::setw(width) << 
		((int64_t)val) << 
		std::dec;
	return ss.str();
}
