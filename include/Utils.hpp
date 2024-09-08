#pragma once

#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace Utils
{
std::string toHexBuffer(std::vector<uint8_t> &buf)
{
    if (buf.size() == 0)
        return std::string();
    std::stringstream ss;
    for (std::size_t i = 0; i < buf.size(); i++)
    {
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((int)buf[i]) << " " << std::dec;
    }
    return ss.str();
}

std::string toHex(uint64_t val, size_t width)
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(width) << ((int64_t)val) << std::dec;
    return ss.str();
}

std::string getTimeAsText(const double time)
{
    std::stringstream out;
    out << std::setfill('0') << std::setw(2) << ((static_cast<int>(time) / (1000 * 60 * 60)) % 24) << ":";
    out << std::setfill('0') << std::setw(2) << ((static_cast<int>(time) / (1000 * 60)) % 60) << ":";
    out << std::setfill('0') << std::setw(2) << ((static_cast<int>(time) / 1000) % 60) << ":";
    out << std::setfill('0') << std::setw(3) << ((static_cast<int>(time) % 1000));
    return out.str();
}

std::string getDelayLine(const float duration)
{
    /*
     * :delay 1000ms;
     */

    return ":delay " + std::to_string((int)duration) + "ms;\n";
}

}; // namespace Utils
