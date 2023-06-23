//  Parser for Velodyne LiDAR VLP-16 data packages
//  References:
//     https://pcapplusplus.github.io/docs/tutorials/packet-parsing
//     https://velodynelidar.com/products/puck

#include <iostream>
#include <array>
#include <vector>
#include "Packet.h"             // basic parsed packet structures
#include <PcapFileDevice.h>     // the API for reading from pcap files
#include "parser.h"

int main(int argc, char* argv[])
{
    // create an interface instance
    pcpp::PcapFileReaderDevice reader(argv[1]);

    // open the reader for reading, verify the file is opened
    if (!reader.open())
    {
        std::cerr << "Cannot open input.pcap for reading" << std::endl;
        return 1;
    }

    // read packets from the file
    pcpp::RawPacket rawPacket;

    if (!reader.getNextPacket(rawPacket))
    {
        std::cerr << "Couldn't read the first packet in the file" << std::endl;
        return 1;
    }

    // creating an instance of the Packet class and giving it in the constructor a pointer to the RawPacket instance
    // parse the raw packet into a parsed packet
    pcpp::Packet parsedPacket;
    // For Data and Position Packet Fields all offsets are given from the Ethernet Layer in the VLP-16 documentation.
    // The Ethernet Layer is the first layer in both types of packet.
    pcpp::Layer* curLayer = nullptr;
    VLPDataPacket vlpPacket;

    // some pcap file statistics
    size_t pos_cnt = 0;
    size_t data_cnt = 0;
    size_t other_cnt = 0;

    uint32_t time_stamp;
    std::vector<Point> scannerCloud;
    std::array<Point, 384> parsedPoints;

    while (rawPacket.isPacketSet())
    {
        if (rawPacket.getFrameLength() == 554) {
            ++pos_cnt;
        }
        else if (rawPacket.getFrameLength() == 1248) {
            parsedPacket.setRawPacket(&rawPacket, false);
            curLayer = parsedPacket.getFirstLayer();
            // Check if we parse VLP-16 Data Packet
            if (ID_VLP16 == *(curLayer->getDataPtr(ID_OFFSET))) {
                ++data_cnt;
                vlpPacket.setPacket(curLayer);
            }
            parsedPoints = vlpPacket.getPoints();
            scannerCloud.insert(std::end(scannerCloud), std::begin(parsedPoints), std::end(parsedPoints));
        }
        else {
            ++other_cnt;
        }
        reader.getNextPacket(rawPacket);
    }

    // close the file reader, we don't need it anymore
    reader.close();

    std::cout << "// Position Packets = " << pos_cnt
              << "\n// Data Packets = " << data_cnt
              << "\n// Other = " << other_cnt
              << std::endl;

    // Output
    std::cout << "// Packet contains: \n";
    std::cout << '{' << std::endl;
    for (auto it = scannerCloud.begin(); it < scannerCloud.end(); ++it) {
        std::cout << *it;
        std::cout << ','  << std::endl;
    }
    std::cout << '}' << std::endl;

    return 0;
}
