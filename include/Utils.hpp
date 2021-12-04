#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

class Utils
{
public:
	static std::string toHexBuffer(std::vector<uint8_t> &buf);
	static std::string toHex(uint64_t val, size_t width);
private:
	Utils();
	~Utils();
};

#endif // UTILS_HPP
