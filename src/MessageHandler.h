#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "TLV.h"

namespace msg
{

    class MessageHandler
    {
    public:
        MessageHandler() = default;
        ~MessageHandler() = default;

        void handle_message(const std::vector<uint8_t> &message);
    };

}
#endif // MESSAGE_HANDLER_H