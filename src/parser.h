#ifndef PARSER_H
#define PARSER_H

#include "TLV.h"

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>

class UartParser
{
public:
    UartParser(boost::asio::io_context &io_context,
               const std::string &port,
               unsigned int baud_rate);
    std::vector<uint8_t> readAndParseUartDoubleCOMPort();

private:
    boost::asio::serial_port m_serial;
    boost::asio::streambuf m_streambuf;
    std::vector<uint8_t> m_magic_word = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};
    void parseStandardFrame(const std::vector<uint8_t> &frameData);
};

#endif // PARSER_H