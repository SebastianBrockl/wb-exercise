#include "frame_identifier.h"
#include "util.h"
#include "MatchPattern.h"
#include <boost/endian/conversion.hpp>
#include <iomanip>

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
    const std::vector<uint8_t> &delimiter)
    : m_serial_port(serial_port),
      m_timeout(serial_port.get_executor(), boost::asio::chrono::seconds(1)),
      m_callback(callback),
      m_delimiter(delimiter)
{

    std::cout << "\nFrame Identifier: Constructed, delimiter: ";
    for (auto it = m_delimiter.begin(); it != m_delimiter.end(); ++it)
    {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*it) << " ";
    }
    std::cout << std::dec << std::endl;
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
        uart::MatchPattern(m_delimiter),
        [self](auto &error, auto bytes_transferred)
        {
            self->handle_frame_start(error, bytes_transferred);
        });
}

/**
 * @brief Handle the start of a frame
 *
 * This function is called when the start of a frame is found. It performs simple sanity checks,
 * manipulates the read buffer to only contain the magic string delimiter and passes controll
 * to the prepare_header function.
 */
void FrameIdentifier::handle_frame_start(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    auto self = shared_from_this();
    if (error)
    {
        std::cerr << "Frame Identifier.handle_frame_start: Error finding frame start: " << error.message()
                  << "\n"
                  << "Frame Identifier.handle_frame_start: bytes read before error: " << bytes_transferred << "\n"
                  << std::endl;
        self->m_callback(error, bytes_transferred);
    }
    else
    {
        auto data = self->m_read_buffer.data();
        const uint8_t *begin = reinterpret_cast<const uint8_t *>(boost::asio::buffer_cast<const char *>(data));
        const uint8_t *end = begin + boost::asio::buffer_size(data);
        // Wrap the buffer in a vector for easier handling.
        std::vector<uint8_t> binary_data(begin, end);

        auto magic_string_size = self->m_delimiter.size();

        auto pair = uart::MatchPattern(self->m_delimiter)(begin, end);
        bool success = pair.second;

        if (!success)
        {
            std::cout << "Frame Identifier.handle_frame_start: Magic string not present.\nThis should never happen!\n"
                      << "Buffer contents: " << self->buffer_as_string() << std::endl;
            return;
        }
        else
        {
            auto one_past_magic_string = pair.first;
            std::cout << "Frame Identifier.handle_frame_start: Magic string found!\n"
                      << "Bytes in buffer: " << self->m_read_buffer.size() << "\n"
                      << "Buffer contents: " << self->buffer_as_string() << "\n"
                      << std::endl;

            std::size_t discarded_bytes = 0;
            // sanity check
            if (one_past_magic_string != std::next(begin, magic_string_size))
            {
                std::cout << "Magic string not at buffer start!" << std::endl;
                discarded_bytes = one_past_magic_string - begin - magic_string_size;
                std::cout << "Discarding " << discarded_bytes << " bytes" << std::endl;
            }
            // if for some reason the magic string is not at the start of the buffer, discard the bytes preceding it.
            self->m_read_buffer.consume(discarded_bytes);
            self->m_read_bytes = self->m_read_buffer.size();
            std::cout << "Bytes in buffer: " << self->m_read_buffer.size() << "\n"
                      << "Buffer contents: " << self->buffer_as_string() << "\n"
                      << std::endl;

            // We can now be certain of that the buffer is alligned with the message start, and start reading the mesage
            // by deserializing the header
            self->prepare_header();
        }
    }
}

/**
 * @brief Make sure the entire header is read in the buffer, and use it to read the rest of the message
 *
 * This function reads the frame header in it's entirety and then passes controll to the read_message function.
 */
void FrameIdentifier::prepare_header()
{
    auto self = shared_from_this();

    size_t header_size = sizeof(FrameHeader);

    if (self->m_read_buffer.size() > header_size)
    {
        // buffer allready contains entire header, no need to read more.
        // auto data = self->get_buffer_data();
        // FrameHeader header;
        try
        {
            auto header = util::deserialize_header(self->get_buffer_data());
            return self->read_message(header.totalPacketLen - self->m_read_buffer.size());
        }
        catch (std::runtime_error &e)
        {
            std::cerr << "Frame Identifier.prepare_header: Error deserializing header: " << e.what() << "\n"
                      << std::endl;
            self->m_callback(boost::asio::error::fault, 0);
            return;
        }
    }
    else
    {

        boost::asio::async_read(
            m_serial_port,
            m_read_buffer,
            boost::asio::transfer_exactly(header_size - self->m_read_buffer.size()),
            [self](const boost::system::error_code &error, std::size_t bytes_transferred)
            {
                if (!error)
                {
                    std::cout << "Frame Identifier.prepare_header: \n"
                              << "Bytes in buffer: " << self->m_read_buffer.size() << "\n"
                              << "Buffer contents: " << self->buffer_as_string() << "\n"
                              << std::endl;

                    // return self->read_message(util::deserialize_header(self->get_buffer_data()).totalPacketLen - self->m_read_buffer.size());

                    auto header = util::deserialize_header(self->get_buffer_data());
                    return self->read_message(header.totalPacketLen - self->m_read_buffer.size());

                    // FrameHeader header = self->deserialize_header();
                    // self->m_read_bytes = self->m_read_buffer.size();
                    // self->read_message(header.totalPacketLen - 40);
                }
                else
                {
                    std::cerr << "Frame Identifier.prepare_header: Error reading header: " << error.message() << "\n"
                              << std::endl;
                    self->m_callback(error, bytes_transferred);
                }
            });
    }
}


void FrameIdentifier::read_message(size_t remaining_message_lenght)
{
    auto self = shared_from_this();

    std::cout << "Frame Identifier.read_message: Reading next: " << remaining_message_lenght << "bytes\n"
              << std::endl;

    boost::asio::async_read(
        m_serial_port,
        m_read_buffer,
        boost::asio::transfer_exactly(remaining_message_lenght),
        [self](const boost::system::error_code &error, std::size_t bytes_transferred)
        {
            if (!error)
            {
                self->m_read_bytes = self->m_read_buffer.size();
                auto data = self->get_buffer_data();
                auto header = util::deserialize_header(data);
                std::cout << "\n"
                          << "Frame Identifier.read_message: Frame received, length: " << self->m_read_bytes << "\n"
                          << util::to_hex_string(header)
                          << std::endl;
                self->m_callback(error, self->m_read_bytes);
            }
            else
            {
                std::cerr << "Frame Identifier.read_message: Error reading sensor data stream: " << error.message() << std::endl;
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

std::string FrameIdentifier::buffer_as_string()
{
    std::stringstream ss;
    std::vector<uint8_t> buffer(m_read_buffer.size());
    boost::asio::buffer_copy(boost::asio::buffer(buffer), m_read_buffer.data());
    return util::to_hex_string(buffer);
}

const std::vector<uint8_t> FrameIdentifier::get_buffer_data()
{
    // buffer allready contains entire header, no need to read more.
    // auto data = self->m_read_buffer.data();
    // const uint8_t *begin = reinterpret_cast<const uint8_t *>(boost::asio::buffer_cast<const char *>(data));
    // const uint8_t *end = begin + boost::asio::buffer_size(data);
    // // Wrap the buffer in a vector for easier handling.
    // const std::vector<uint8_t> binary_data(begin, end);

    std::vector<uint8_t> buffer(m_read_buffer.size());
    boost::asio::buffer_copy(boost::asio::buffer(buffer), m_read_buffer.data());
    return buffer;
}
