#include <iostream>
#include "parser.h"

std::ostream& operator<<(std::ostream& out, const Point& pt){

    out << "  {"  << std::endl;
    out << "    \"time\": "         << pt.point_time   << ',' << std::endl;
    out << "    \"distance\": "     << pt.radius       << ',' << std::endl;
    out << "    \"elevation\" : "   << pt.elevation    << ',' << std::endl;
    out << "    \"azimuth\": "      << pt.azimuth      << ',' << std::endl;
    out << "    \"reflectivity\": " << pt.reflectivity        << std::endl;
    out << "  }";
    return out;
}

//
VLPDataPacket::VLPDataPacket() {
    init_time_offset();
}

//
VLPDataPacket::VLPDataPacket(pcpp::Layer* curLayer) {
    init_time_offset();
    setPacket(curLayer);
}

//
void VLPDataPacket::setPacket(pcpp::Layer* curLayer)
{
    double curAthimuth;
    double nextAthimuth;
    double azimuthGap;
    // pointer to the TimeStamp
    ptr = curLayer->getDataPtr(DATA_TIMESTAMP_OFFSET);
    // When getting data larger than 1 byte we need to convert them
    // from network to host byte order as the raw packet arrives in network order
    TimeStamp = double(*(ptr + 3) << 24 | *(ptr + 2) << 16 | *(ptr + 1) << 8  | *ptr);

    // pointer to the flag of the first block
    ptr = curLayer->getDataPtr(HEADER_LENGTH);
    // Get athimuth of the first block
    curAthimuth = double(*(ptr + AZIMUTH_OFFSET + 1) << 8 | *(ptr + AZIMUTH_OFFSET)) / 100.0;

    // Iterate over Blocks 0 -> 11
    for (size_t iBlock = 0; iBlock < nBlock; ++iBlock) {
        // cannot get the nextAthimuth from the last block
        // so for the last block the previous gap will be used
        if (iBlock < (nBlock - 1)) {
            nextAthimuth = double(*(ptr + AZIMUTH_OFFSET + BLOCK_LENGTH + 1) << 8 |
                                  *(ptr + AZIMUTH_OFFSET + BLOCK_LENGTH)) / 100.0;
            // adjust for an Azimuth rollover from 359.99° to 0°
            if (nextAthimuth < curAthimuth) {
                nextAthimuth += 360;
            }
            azimuthGap = nextAthimuth - curAthimuth;
        }
        // Iterate over Points 0 -> 31
        for (size_t iPoint = 0; iPoint < nPoint; ++iPoint) {
            // Athimuth (dgr)
            // Perform the interpolation using the timing firing
            // More sophisticated interpolation possible (polynomial or spline)
            // if read all 12 azimuth values before
            // Determine if you’re in the first or second firing sequence of the data block
            if (iPoint < 16) {
                packPoints[iBlock * nPoint + iPoint].azimuth = curAthimuth +
                                                               azimuthGap * (2.304 * iPoint / (2 * 55.296));

            }
            else {
                packPoints[iBlock * nPoint + iPoint].azimuth = curAthimuth +
                                                               azimuthGap * (55.296 + 2.304 * (iPoint - 16)) / (2 * 55.296);
            }
            // Adjust for any rollover
            if (packPoints[iBlock * nPoint + iPoint].azimuth >= 360) {
                packPoints[iBlock * nPoint + iPoint].azimuth -= 360;
            }
            // Radius (distance) (m)
            // The distance is an unsigned integer. It has 2 mm granularity. (2mm/1000mm = 1/500 m)
            packPoints[iBlock * nPoint + iPoint].radius =
                double(*(ptr + iPoint * POINT_STEP + RADIUS_OFFSET + 1) << 8  |
                       *(ptr + iPoint * POINT_STEP + RADIUS_OFFSET)) / 500.0;
            // Reflectivity
            packPoints[iBlock * nPoint + iPoint].reflectivity = *(ptr + iPoint * POINT_STEP + REFLECTIVITY_OFFSET);
            // Elevation (dgr)
            // from the firing sequences
            packPoints[iBlock * nPoint + iPoint].elevation = VertAngel[iPoint % 16];
            // Time (us)
            packPoints[iBlock * nPoint + iPoint].point_time = TimeStamp + time_offset[iBlock][iPoint];
        }
        curAthimuth = nextAthimuth;
        // move ptr to the athimuth of the next block
        ptr += BLOCK_LENGTH;
    }
}

//
void VLPDataPacket::init_time_offset() {
    for (size_t iBlock = 0; iBlock < nBlock; ++iBlock) {
        for (size_t iPoint = 0; iPoint < nPoint; ++iPoint) {
            if (iPoint < 16) {
                time_offset[iBlock][iPoint] = 55.296 * 2 * iBlock + 2.304 * iPoint;
            }
            else {
                time_offset[iBlock][iPoint] = 55.296 * (2 * iBlock + 1) + 2.304 * (iPoint - 16);
            }
        }
    }
}
