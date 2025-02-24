#include "frame_identifier.h"

/**
 * @brief Construct a new Frame Identifier object
 *
 * @param serial_port The serial port to read from
 * @param callback The callback to call when a frame is found
 * @param delimiter The delimiter to search for
 *
 * This constructor initializes the frame identifier with the serial port, callback and delimiter
 * and starts a timeout.
 * Example usage:
 * @code
 * auto frame_identifier = std::make_shared<FrameIdentifier>(serial_port, callback, delimiter);
 * frame_identifier->start();
 */
FrameIdentifier::FrameIdentifier(
    boost::asio::serial_port &serial_port,
    callback_t callback,
    const std::string &delimiter)
    : m_serial_port(serial_port),
      m_timeout(serial_port.get_executor(), boost::asio::chrono::seconds(1)),
      m_callback(callback),
      m_delimiter(delimiter)
{
}

/**
 * @brief Start the frame identifier. REMEMBER TO CALL AFTER CONSTRUCTION!
 *
 * This function starts the frame identifier by starting a timeout and then calling find_frame_start
 */
void FrameIdentifier::start()
{
    auto self = shared_from_this();
    self->start_timeout();
    find_frame_start();
}

void FrameIdentifier::find_frame_start()
{
    auto self = shared_from_this();

    boost::asio::async_read_until(
        m_serial_port,
        m_read_buffer,
        m_delimiter,
        [self](const boost::system::error_code &error, std::size_t bytes_transferred)
        {
            if (!error)
            {
                std::istream is(&self->m_read_buffer);
                std::string data;
                std::getline(is, data);

                std::size_t magic_string_position = data.find(self->m_delimiter);
                if (magic_string_position == std::string::npos)
                {
                    // sanity check
                    std::cout << "Data UART: Magic string not found this should never happen" << std::endl;
                    return;
                }
                if (magic_string_position != std::string::npos)
                {
                    // clear the buffer except for the magic string
                    self->m_read_buffer.consume(magic_string_position);
                    self->m_read_bytes = self->m_read_buffer.size();
                    self->read_header();
                }
            }
            else
            {
                std::cerr << "Data UART: Error reading sensor data stream: " << error.message()
                          << "\n"
                          << "Data UART: bytes read: " << bytes_transferred << std::endl;
                self->m_callback(error, bytes_transferred);
            }
        });
}

void FrameIdentifier::read_header()
{
    auto self = shared_from_this();
    // read next 32 bytes composing the message header

    boost::asio::async_read(
        m_serial_port,
        m_read_buffer,
        boost::asio::transfer_exactly(32),
        [self](const boost::system::error_code &error, std::size_t bytes_transferred)
        {
            if (!error)
            {
                FrameHeader header = self->deserialize_header();
                self->m_read_bytes = self->m_read_buffer.size();
                self->read_message(header.totalPacketLen - 40);
            }
            else
            {
                std::cerr << "Frame Identifier: Error reading sensor data stream: " << error.message() << std::endl;
                self->m_callback(error, bytes_transferred);
            }
        });
}

FrameHeader FrameIdentifier::deserialize_header()
{
    FrameHeader header;
    std::istream is(&m_read_buffer);
    is.read(reinterpret_cast<char *>(&header), sizeof(FrameHeader));
    return header;
}

void FrameIdentifier::read_message(size_t remaining_message_lenght)
{
    auto self = shared_from_this();
    boost::asio::async_read(
        m_serial_port,
        m_read_buffer,
        boost::asio::transfer_exactly(remaining_message_lenght),
        [self](const boost::system::error_code &error, std::size_t bytes_transferred)
        {
            if (!error)
            {
                self->m_read_bytes = self->m_read_buffer.size();
                auto header = self->deserialize_header();
                std::cout 
                    << "Frame Identifier: Frame received, length: " << self->m_read_bytes << "\n"
                    << "Frame Identifier: header magic word: " << header.magic_word << "\n"
                    << "Frame Identifier: header version : " << header.version << "\n"
                    << "Frame Identifier: header total packet lenght : " << header.totalPacketLen << "\n"
                    << "Frame Identifier: header platform : " << header.platform << "\n"
                    << "Frame Identifier: header frame number : " << header.frameNumber << "\n"
                    << "Frame Identifier: header time stamp : " << header.timeCpuCycles << "\n"
                    << "Frame Identifier: num detected obj: " << header.numDetectedObj << "\n"
                    << "Frame Identifier: num TLV : " << header.numTLVs << "\n"
                    << "Frame Identifier: subframe num : " << header.subFrameNumber << "\n"
                    << std::endl;
                self->m_callback(error, self->m_read_bytes);
            }
            else
            {
                std::cerr << "Frame Identifier: Error reading sensor data stream: " << error.message() << std::endl;
                self->m_callback(error, bytes_transferred);
            }
        });
}

void FrameIdentifier::read_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    auto self = shared_from_this();
    if (error)
    {
        std::cout << "Frame Identifier: Error reading sensor data stream: " << error.message() << std::endl;
        self->m_callback(error, bytes_transferred);
    }
}

std::size_t FrameIdentifier::match_magic_string(boost::asio::streambuf &readBuffer)
{
    auto data = boost::asio::buffer_cast<const uint8_t *>(readBuffer.data());
    auto bufferSize = readBuffer.size();

    auto magic_string = m_delimiter.c_str();
    auto magic_string_lenght = strlen(magic_string);

    // compare readbuffer against magic string , returns position of start of message frame header
    for (std::size_t i = 0; i + magic_string_lenght <= bufferSize; ++i)
    {
        if (std::memcmp(data + i, magic_string, magic_string_lenght) == 0)
        {
            std::cout << "Frame Identifier: Magic string found" << std::endl;
            return i + magic_string_lenght;
        }
    }
    return 0; // not found
}

/**
 * @brief Start a timeout to cancel the read operation
 *
 * This function starts a timeout to cancel the read operation if the start of a frame is not found
 * within the timeout period, 1 second.
 */
void FrameIdentifier::start_timeout()
{
    auto self = shared_from_this();
    m_timeout.async_wait(
        [self](const boost::system::error_code &error)
        {
            if (!error)
            {
                self->m_serial_port.cancel();
            }
        });
}
