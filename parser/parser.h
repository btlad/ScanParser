#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <array>
#include "Layer.h"

// Data Packet Structure Fields and Offsets
// Time Stamp
#define DATA_TIMESTAMP_OFFSET     0x04DA
#define TIMESTAMP_LENGTH               4
// Header length in bytes
#define HEADER_LENGTH                 42
#define BLOCK_LENGTH                 100
#define AZIMUTH_OFFSET                 2
#define RADIUS_OFFSET                  4
#define REFLECTIVITY_OFFSET            6
#define POINT_STEP                     3

// Factory Bytes, Return Modes
#define MODE_OFFSET               0x04DE
#define MODE_STRONGEST              0x37  /* 55 */
#define MODE_LAST                   0x38  /* 56 */
#define MODE_DUAL                   0x39  /* 57 */

// Factory Bytes, Product ID
#define ID_OFFSET                 0x04DF
#define ID_VLP16                    0x22  /* 34 */

// Position Packet Structure Field Offsets
#define POSITION_TIMESTAMP_OFFSET   0xF0

struct Point
{
    // In the VLP-16 documentation a non-standard spherical coordinates are used
    //   - radius    (r) as in standard;
    //   - elevation (ω) angle between x-y plane and r;
    //   - azimuth   (α) clockwise angle from y to x axis
    double    radius;
    double    elevation;
    double    azimuth;
    uint16_t  reflectivity;
    double    point_time;
};

std::ostream& operator<<(std::ostream&, const Point&);

class VLPDataPacket
{
private:
    static const size_t nChan  = 16;
    static const size_t nPoint = 32;
    static const size_t nBlock = 12;

    // lasers are organized in a single, vertical column, they are not fired from one end to the other.
    // Instead, the firing sequence "hops around." This is to avoid "cross-talk" or interference.
    const std::array<int16_t, nChan> VertAngel = { -15, 1 ,-13, 3, -11, 5, -9, 7, -7, 9, -5, 11, -3, 13, -1, 15};

    // Timing offsets of every point in a packet
    // 12 blocks, 2 laser firings in block, 16 point in firing sequences
    // (12x2x16 = 12x32 = 384)
    std::array<std::array<double, nPoint>, nBlock> time_offset;
    std::array<Point, 384> packPoints;

    //  The four-byte time stamp is a 32-bit unsigned integer marking the moment
    //  of the first data point in the first firing sequence of the first data block.
    //  The time stamp’s value is the number of microseconds elapsed since the top of the hour.
    //  The number ranges from 0 to 3,599,999,999, the number of microseconds in one hour.
    double TimeStamp{};

    // pointer to the data fields in the packet
    uint8_t*  ptr = nullptr;

    // All 16 lasers are fired and recharged every 55.296 μs.
    // The time between firings is 2.304 μs.
    // There are 16 firings (16 × 2.304 μs) followed by an idle period of 18.43 μs.
    // Therefore, the timing cycle to fire and recharge all 16 lasers is given by
    // (16 × 2.304 μs) + 18.432 μs = 55.296 μs.
    void init_time_offset();

public:
    VLPDataPacket();
    VLPDataPacket(pcpp::Layer* curLayer);
    ~VLPDataPacket() = default;

    VLPDataPacket(VLPDataPacket &) = delete;
    VLPDataPacket& operator=(VLPDataPacket&) = delete;

    void setPacket(pcpp::Layer* curLayer);
    std::array<Point, 384> getPoints() { return packPoints; };

};

#endif // PARSER_H
