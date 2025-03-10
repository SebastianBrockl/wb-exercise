#include "MessageHandler.h"
#include "util.h"
#include "TLV.h"
#include <iostream>

using namespace msg;

/**
 * @brief Construct a new Message Handler object
 *
 * @param message_data unique pointer to a vector of uint8_t containing the message data bytes
 *                     The message data is expected to be a complete message, including the header
 *                     the ownership of the message data is transferred to the MessageHandler
 */
MessageHandler::MessageHandler(std::unique_ptr<std::vector<uint8_t>> message_data)
    : m_message_bytes(std::move(message_data))
{
}

std::vector<tlv> MessageHandler::deserialize_message_data(const std::vector<uint8_t> &message_data)
{
    if (message_data.size() < sizeof(FrameHeader))
    {
        auto message =
            std::string("MessageHandler: Message data too short for header deserialization! ") + "Expected at least " + std::to_string(sizeof(FrameHeader)) + " bytes, got " + std::to_string(message_data.size());
        throw std::runtime_error(message);
    }
    std::vector<tlv> tlvs;
    // grab first 40 bytes to form header
    auto iterator = message_data.begin();
    auto header = util::deserialize_header(message_data, iterator);
    m_frame_header = header;
    m_message_contents.push_back(header);

    return tlvs;
}

void MessageHandler::handle_message(const std::vector<uint8_t> &message)
{
    // auto tlvs = deserialize_message_data(message);
    // for (const auto &tlv : tlvs)
    // {
    //     std::visit([](auto &&arg) {
    //         using T = std::decay_t<decltype(arg)>;
    //         if constexpr (std::is_same_v<T, CompressedPointCloudTLV>)
    //         {
    //             std::cout << "Compressed Point Cloud TLV" << std::endl;
    //         }
    //         else if constexpr (std::is_same_v<T, TargetListTLV>)
    //         {
    //             std::cout << "Target List TLV" << std::endl;
    //         }
    //         else if constexpr (std::is_same_v<T, targetIndexTLV>)
    //         {
    //             std::cout << "Target Index TLV" << std::endl;
    //         }
    //         else if constexpr (std::is_same_v<T, PresenceIndicationTLV>)
    //         {
    //             std::cout << "Presence Indication TLV" << std::endl;
    //         }
    //         else if constexpr (std::is_same_v<T, TargetHeightTLV>)
    //         {
    //             std::cout << "Target Height TLV" << std::endl;
    //         }
    //         else
    //         {
    //             static_assert(always_false<T>::value, "non-exhaustive visitor!");
    //         }
    //     },
    //                tlv);
    // }
}