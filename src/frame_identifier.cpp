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
    const std::string& delimiter)
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
void FrameIdentifier::start() {
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
            self->m_read_bytes = self->m_read_bytes + bytes_transferred;
            if (!error)
            {
                std::cout << "Data UART: Magic string found" << std::endl;
                self->find_frame_start();
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
