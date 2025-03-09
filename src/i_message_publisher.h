#ifndef I_MESSAGE_PUBLISHER_H
#define I_MESSAGE_PUBLISHER_H

#include "TLV.h"

namespace msg
{
    class IMessagePublisher
    {
    public:
        virtual ~IMessagePublisher() = default;

        virtual void publish_frame(const Frame &frame) = 0;

        virtual void publish_pointcloud(const CompressedPointCloudTLV &pointcloud) = 0;
        virtual void publish_target_list(const TargetListTLV &target_list) = 0;
        virtual void publish_target_index(const targetIndexTLV &target_index) = 0;
        virtual void publish_presence_indication(const PresenceIndicationTLV &presence_indication) = 0;
        virtual void publish_target_height(const TargetHeightTLV &target_height) = 0;
    };
}

#endif // I_MESSAGE_PUBLISHER_H
