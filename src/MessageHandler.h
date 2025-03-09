#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "TLV.h"
#include <vector>
#include <memory>

namespace msg
{

    class MessageHandler
    {
    public:
        MessageHandler(std::unique_ptr<std::vector<uint8_t>> message_data);
        ~MessageHandler() = default;

        void handle_message(const std::vector<uint8_t> &message);
        // deserialize message bytes to vector of tlv structs
        std::vector<tlv> deserialize_message_data(const std::vector<uint8_t> &message_data);

    private:
        std::unique_ptr<std::vector<uint8_t>> m_message_bytes;
        std::vector<tlv> m_message_contents;
        FrameHeader m_frame_header;
    };

}
#endif // MESSAGE_HANDLER_H