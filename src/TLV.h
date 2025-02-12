#ifndef TLV_H
#define TLV_H

#include <cstdint>
#include <variant>
#include <vector>

#pragma pack(push, 1) // Ensure no padding
struct FrameHeader
{
    uint64_t magic_word;     // syncPattern in hex is: ‘02 01 04 03 06 05 08 07’
    uint32_t version;        // software version
    uint32_t totalPacketLen; // total packet length
    uint32_t platform;       // platform type
    uint32_t frameNumber;    // frame number
    uint32_t timeCpuCycles;  // time in CPU cycles
    uint32_t numDetectedObj; // number of detected objects
    uint32_t numTLVs;        // number of TLVs in this frame
    uint32_t subFrameNumber; // sub-frame number
};
#pragma pack(pop)

/**
 * TLV Header
 */
#pragma pack(push, 1) // Ensure no padding
struct TlvHeader
{
    // TLV Type:
    // 1020 = Point Cloud
    // 1010 = Target object list
    // 1011 = Target Index
    // 1012 = Target Height
    // 1021 = Presence Indication
    uint32_t type;   // type of TLV
    uint32_t length; // length of TLV
};
#pragma pack(pop)

/**
 * Used to decompress the point cloud data
 * the size of the TLV is sizeof(TlvHeader) + sizeof(PointUnit) * numPoints
 */
#pragma pack(push, 1) // Ensure no padding
struct PointUnit
{
    float elevationUnit; // elevation unit - Multiply each point by this value - used for compression
    float azimuthUnit;   // azimuth unit - Multiply each point by this value - used for compression
    float dopplerUnit;   // doppler unit - Multiply each point by this value - used for compression
    float rangeUnit;     // range unit - Multiply each point by this value - used for compression
    float snrUnit;       // snr unit - Multiply each point by this value - used for compression
};
#pragma pack(pop)

#pragma pack(push, 1) // Ensure no padding
struct PointStruct
{
    int8_t elevation; // elevation
    int8_t range;     // range
    int16_t azimuth;  // azimuth
    int16_t doppler;  // doppler
    int16_t snr;      // snr
};
#pragma pack(pop)

/**
 * TLV size: sizeof (tlvHeaderStruct) + sizeof (trackerProc_Target) x numberOfTargets
 * he Target List TLV consists of an array of targets. Each target object is defined as given below.
 */
#pragma pack(push, 1) // Ensure no padding
struct Target
{
    uint32_t trackId;      // track id
    float posx;            // target x position, meters
    float posy;            // target y position, meters
    float posz;            // target z position, meters
    float velx;            // target x velocity, m/s
    float vely;            // target y velocity, m/s
    float velz;            // target z velocity, m/s
    float accx;            // target x acceleration, m/s^2
    float accy;            // target y acceleration, m/s^2
    float accz;            // target z acceleration, m/s^2
    float ec[16];          // Tracking error covariance matrix, [4x4] in range/azimuth/elevation/doppler coordinates
    float g;               // Gating function gain
    float confidencelevel; // Confidence level
};
#pragma pack(pop)

/**
 * Size: sizeof (tlvHeaderStruct) + sizeof(uint8) x numberOfPoints (NOTE: here the number of points are for frame n-1)
 * The Target Index TLV consists of an array of target IDs.
 * A targetID at index i is the target to which point i of the previous frame’s point cloud was associated. Valid IDs range from 0-249.
 *
 * Special values:
 * 253: Point was not associated with any target
 * 254: Point not associated, located outside boundary of interest
 * 255: Point not associated, considered noise
 */
#pragma pack(push, 1) // Ensure no padding
struct TargetIndex
{
    uint8_t trackId; // target id
};
#pragma pack(pop)

/**
 * Size: sizeof(tlvHeaderStruct) + sizeof(uint32)
 * The Presence Indication TLV consists of a single uint32 corresponding to the number of presences detected.
 */
#pragma pack(push, 1) // Ensure no padding
struct Precences
{
    uint32_t presences; // number of precences detected
};
#pragma pack(pop)

/**
 * Size: sizeof(tlvHeaderStruct) + (sizeof(targetHeight)) x numberOfTargets
 *
 * The Target Height TLV consists of a single uint8 corresponding to the track number it refers to,
 * then the maximum Z estimate given as a float and the minimum Z estimate given as a float.
 * For a more detailed explanation of the height estimation algorithm, see the 3D People Tracking Implementation Guide.
 */
#pragma pack(push, 1) // Ensure no padding
struct TargetHeight
{
    uint8_t trackId; // track id
    float maxZ;      // maximum height estimate
    float minZ;      // minimum height estimate
};
#pragma pack(pop)

#pragma pack(push, 1) // Ensure no padding
struct CompressedPointCloudTLV
{
    TlvHeader header;
    PointUnit pointUnit;
    PointStruct points[];
};
#pragma pack(pop)

#pragma pack(push, 1) // Ensure no padding
struct TargetListTLV
{
    TlvHeader header;
    Target targets[];
};
#pragma pack(pop)

#pragma pack(push, 1) // Ensure no padding
struct targetIndexTLV
{
    TlvHeader header;
    TargetIndex targetIndex[];
};
#pragma pack(pop)

#pragma pack(push, 1) // Ensure no padding
struct PresenceIndicationTLV
{
    TlvHeader header;
    Precences presences;
};
#pragma pack(push, 1) // Ensure no padding

#pragma pack(push, 1) // Ensure no padding
struct TargetHeightTLV
{
    TlvHeader header;
    TargetHeight targetHeight[];
};
#pragma pack(pop)

using TLVs = std::variant<CompressedPointCloudTLV, TargetListTLV, targetIndexTLV, PresenceIndicationTLV, TargetHeightTLV>;
struct Frame
{
    FrameHeader header;
    std::vector<TLVs> tlvs;
};

#endif // TLV_H