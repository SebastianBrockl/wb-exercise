#include "data_uart.h"

DataUART::DataUART(boost::asio::io_context& io_context, const std::string& port, uint32_t baud_rate)
    : m_serial_port(io_context, port), m_strand(io_context.get_executor()) {
    m_serial_port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    std::cout << "Data UART initialized" << std::endl;
    std::cout << "Data UART Port: " << port << std::endl;
    std::cout << "Data UART Baud Rate: " << baud_rate << std::endl;
}

void DataUART::start_async_read() {
    std::cout << "Data UART: Starting async read" << std::endl;
    boost::asio::async_read(m_serial_port, boost::asio::buffer(m_read_buffer.prepare(1024)),
        boost::asio::bind_executor(m_strand,
            std::bind(&DataUART::handle_read, this, std::placeholders::_1, std::placeholders::_2)));
}

/**
 * @brief Callback function for async_read
 * 
 * @param error Error code from the async_read operation
 * @param bytes_transferred Number of bytes transferred
 * 
 * This function is called when the async_read operation completes. It reads the data from the streambuf
 * and searches for the magic string that indicates the start of a frame. When the magic string is found,
 * the frame is collected until the full frame is received. The frame is then passed to the handle_frame
 * function for further processing.
 * 
 * Start of frame header looks like this:
 *  uint64_t magic_word;     // syncPattern in hex is: ‘02 01 04 03 06 05 08 07’
 *  uint32_t version;        // software version
 *  uint32_t totalPacketLen; // total packet length
 * 
 */
void DataUART::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred) {
    //TODO remove this debug log
    std::cout << "Data UART: Handling read" << std::endl;
    std::cout << "Data UART: Bytes transferred: " << bytes_transferred << std::endl;
    if (!error) {
        m_read_buffer.commit(bytes_transferred);
        std::istream is(&m_read_buffer);
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

        // Magic string to identify the start of a frame
        static const std::vector<uint8_t> magic_string = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};

        // If not already collecting a frame, search for the magic string
        if (!m_collecting_frame) {
            auto it = std::search(data.begin(), data.end(), magic_string.begin(), magic_string.end());
            // If the magic string is found, start collecting the frame in m_frame_buffer
            if (it != data.end()) {
                std::cout << "Data UART: Magic string encountered" << std::endl;
                m_collecting_frame = true;
                // TODO handle frame overshoot here, potential overshoot will cause frames to be missed
                m_frame_buffer.insert(m_frame_buffer.end(), it, data.end());
            }
        }

        // while collecting a frame, add data to the frame buffer
        if (m_collecting_frame) {
            m_frame_buffer.insert(m_frame_buffer.end(), data.begin(), data.end());

            if (m_frame_buffer.size() >= 16) {
                if (m_frame_length == 0) {
                    m_frame_length = extractUint32(m_frame_buffer, 12);
                }

                if (m_frame_buffer.size() >= m_frame_length) {
                    // TODO optimize this
                    auto complete_frame = std::make_shared<std::vector<uint8_t>>(std::move(m_frame_buffer));
                    handle_frame(complete_frame);
                    m_frame_buffer.clear();
                    m_collecting_frame = false;
                    m_frame_length = 0;
                }
            }
        }

        start_async_read();
    } else {
        std::cerr << "Data UART: Error reading sensor data stream: " << error.message() << std::endl;
    }
}

void DataUART::handle_frame(std::shared_ptr<std::vector<uint8_t>> frame) {
    std::cout << "Data UART: Frame received, length: " << frame->size() << std::endl;
    //TODO
    // Send the frame to the parser for deserialization
    // something like: parser.deserialize(frame);
}

/**
 * @brief Extracts a uint32_t from a buffer at a given offset
 * 
 * @param buffer The buffer to extract the uint32_t from
 * @param offset The offset in the buffer to start extracting the uint32_t
 * @return uint32_t The extracted uint32_t
 * 
 * Helper method for (hopefully) safely extracting the frame length from the frame header.
 */
uint32_t DataUART::extractUint32(const std::vector<uint8_t>& buffer, size_t offset) {
    uint32_t value;
    std::memcpy(&value, &buffer[offset], sizeof(uint32_t)); // Safe memory copy
    return value;
}