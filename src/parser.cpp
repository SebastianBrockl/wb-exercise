#include "parser.h"
#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <cstring>

UartParser::UartParser(boost::asio::io_context& io_context, const std::string& port, unsigned int baud_rate)
    : m_serial(io_context, port) {
    m_serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
}

std::vector<uint8_t> UartParser::readAndParseUartDoubleCOMPort() {
    std::vector<uint8_t> frameData;
    uint8_t magicByte;
    size_t index = 0;

    while (true) {
        boost::asio::read(m_serial, m_streambuf.prepare(1));
        m_streambuf.commit(1);
        std::istream is(&m_streambuf);
        is.read(reinterpret_cast<char*>(&magicByte), 1);

        if (magicByte == m_magic_word[index]) {
            index++;
            frameData.push_back(magicByte);
            if (index == 8) {
                break; // Found the full magic word
            }
        } else {
            if (index == 0) {
                continue;
            }
            index = 0;
            frameData.clear();
        }
    }

    // Read in version from the header
    uint8_t versionBytes[4];
    boost::asio::read(m_serial, m_streambuf.prepare(4));
    m_streambuf.commit(4);
    std::istream is_version(&m_streambuf);
    is_version.read(reinterpret_cast<char*>(versionBytes), 4);
    frameData.insert(frameData.end(), versionBytes, versionBytes + 4);

    // Read in length from header
    uint8_t lengthBytes[4];
    boost::asio::read(m_serial, m_streambuf.prepare(4));
    m_streambuf.commit(4);
    std::istream is_length(&m_streambuf);
    is_length.read(reinterpret_cast<char*>(lengthBytes), 4);
    frameData.insert(frameData.end(), lengthBytes, lengthBytes + 4);
    uint32_t frameLength = *reinterpret_cast<uint32_t*>(lengthBytes);

    // Subtract bytes that have already been read (magic word, version, and length)
    frameLength -= 16;

    // Read in rest of the frame
    boost::asio::read(m_serial, m_streambuf.prepare(frameLength));
    m_streambuf.commit(frameLength);
    std::istream is_rest(&m_streambuf);
    std::vector<uint8_t> restOfFrame(frameLength);
    is_rest.read(reinterpret_cast<char*>(restOfFrame.data()), frameLength);
    frameData.insert(frameData.end(), restOfFrame.begin(), restOfFrame.end());

    // frameData now contains an entire frame, send it to parser
    parseStandardFrame(frameData);

    return frameData;
}

void UartParser::parseStandardFrame(const std::vector<uint8_t>& frameData) {
    // Implement the frame parsing logic here
    std::cout << "Parsing frame data..." << std::endl;
    // Example: Print the frame data
    for (const auto& byte : frameData) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
}