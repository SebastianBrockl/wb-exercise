#include "config_uart.h"

ConfigUART::ConfigUART(
    boost::asio::io_context &io_context,
    const std::string &port,
    uint32_t baud_rate)
    : m_serial_port(io_context, port),
      io_context(io_context),
      m_strand(io_context.get_executor()),
      m_30ms_delay(io_context, boost::asio::chrono::milliseconds(30))
{
    m_serial_port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    std::cout << "Config UART initialized" << "\n";
    std::cout << "Config UART Port: " << port << "\n";
    std::cout << "Config UART Baud Rate: " << baud_rate << std::endl;
}

void ConfigUART::async_write_config(const std::string &config_data,
                                    callback_t callback)
{
    bool expected = false;
    if (m_is_transmitting.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
        ready_transmission(callback);

        // split config data into lines
        std::istringstream iss(config_data);
        std::string line;
        while (std::getline(iss, line))
        {
            m_transmit.push_back(line.append("\n"));
        }
        std::cout << "Config UART: Starting configuration Transmission" << std::endl;

        write_next_line(m_transmit[m_sentLines]);
        return;
    }
    else
    {
        std::cout << "Config UART: Transmission already in progress" << std::endl;
    }
}

void ConfigUART::transmission_error(const boost::system::error_code &errorCode)
{
    std::cout
        << "\n"
        << "Config UART: Config transmission aborted due to error: " << errorCode.message()
        << "\n"
        << "Config UART: Sent lines: " << m_sentLines << " of " << m_transmit.size()
        << "\n"
        << "Config UART: Received lines: " << m_receivedLines << std::endl;

    for (const auto &line : m_receive)
    {
        std::cout << line << std::endl;
    }
    std::cout << std::endl;

    m_is_transmitting.store(false, std::memory_order_relaxed);

    // call the write callback callback with the received error
    m_transmission_callback(errorCode, 0);
}

void ConfigUART::transmission_complete()
{
    // all lines have been sent successfully
    std::cout
        << "\n"
        << "Config UART: Config transmission completed successfully: "
        << "\n"
        << "\n";

    for (const auto &line : m_receive)
    {
        std::cout << line << "\n";
    }
    std::cout << std::endl;

    size_t sentBytes = 0;
    for (const auto &line : m_transmit)
    {
        sentBytes += line.size();
    }
    m_is_transmitting.store(false, std::memory_order_relaxed);
    m_transmission_callback(boost::system::error_code(), sentBytes);
}

/**
 * @brief Prepare for transmission by clearing buffers and setting callback
 *
 * @param callback callback function to call when transmission is completed successfully, or with error
 */
void ConfigUART::ready_transmission(callback_t callback)
{
    // state management:
    m_buffer.consume(m_buffer.size());
    m_receive.clear();
    m_transmit.clear();
    m_acks.clear();
    m_sentLines = 0;
    m_transmission_callback = callback;
}

/**
 * @brief Delay 30 ms then write an config line in it's entirety
 *
 * @param line config line which to write over UART
 * @param callback callback function to call when write is completed successfully or with error
 */
void ConfigUART::write_next_line(const std::string &line)
{
    if (m_sentLines >= m_transmit.size())
    {
        transmission_complete();
        return;
    }
    // NOTE: theres three nested lambdas here, but we're chaining async operations, so thers not really a cleaner way to do this
    // first wait 30ms, then write line, then read ack
    // async wait 30ms
    m_30ms_delay.async_wait(
        [this, line](auto &delayError)
        {
            if (delayError)
            {
                // call the write callback callback with the received error
                transmission_error(delayError);
            }
            else
            {
                // otherwise write line
                io_context.post(
                    [this, line]()
                    { 
                        std::cout << "\nConfig UART: tx: " << line << std::endl;
                        boost::asio::async_write(
                          m_serial_port,
                          boost::asio::buffer(line),
                          [this](auto writeError, auto io)
                          {
                                        ++m_sentLines;
                                        read_ack(writeError, io); }); });
            }
        });
}

/**
 * @brief Read ack from UART, two acks are expected for each line written
 *
 * @param errorCode Error code from the async_write operation
 * @param ioBytes Number of bytes written
 *
 * This function is called when the async_write operation completes. It reads the ack from the streambuf
 * and stores it in a temporary buffer. When two acks have been received, the acks are stored in the m_receive
 * buffer and the next line is written.
 */
void ConfigUART::read_ack(const boost::system::error_code &errorCode, std::size_t ioBytes)
{
    boost::asio::async_read_until(
        m_serial_port,
        m_buffer, // Buffer into which data is read
        '\n',     // Delimiter which terminates read operation
        [this](auto &readError, std::size_t bytes_transferred)
        {
            // callback called once async read is terminated
            if (readError)
            {
                std::cout << "Config UART: Error reading ack: " << readError.message() << std::endl;
                transmission_error(readError);
            }
            else
            {
                // read ack and store in temporary m_acks "buffer"
                std::istream is(&m_buffer);
                std::string ack;
                std::getline(is, ack);

                // remove carriage return for prettier log output
                ack.erase(std::remove(ack.begin(), ack.end(), '\r'), ack.end());

                // special handling for the sensorStop ack, which unlike others is three lines
                // NOTE: not pretty, but necessary if using async_read_until
                if (ack.find("Ignored") != std::string::npos && m_acks.size() == 1)
                {
                    m_acks[0] = m_acks[0] + ack;
                }
                else
                {
                    m_acks.push_back(ack);
                }

                std::cout << "Config UART: rx: " << ack << std::endl;

                if (m_acks.size() >= 2)
                {
                    // if two acks have been received, prepare for next line
                    ++m_receivedLines;
                    m_receive.insert(m_receive.end(), m_acks.begin(), m_acks.end());

                    m_acks.clear();

                    write_next_line(m_transmit[m_sentLines]);
                }
                else
                {
                    read_ack(readError, bytes_transferred); // one more ack to read for this line
                }
            }
        }

    );
}
