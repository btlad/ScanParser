# Scan Parser

This C++ code retrieves Velodyne LiDAR VLP-16 data from network packets contained in pcap files and redirect them to standard output formatted in a json-like format

1. Depends on :

  - g++;
  - cmake;
  - pcapplusplus;

2. Example input [`my.pcap`](./data/my.pcap) and output result [`my.json`](./data/my.json) files are in the data folder.

### References:

  + [PcapPlusPlus Tutorial](https://pcapplusplus.github.io/docs/tutorials/packet-parsing)
  + [ArchLinux AUR package](https://aur.archlinux.org/packages/pcapplusplus)
  + [FindPCAP.cmake](https://github.com/seladb/PcapPlusPlus)
  + Velodyne’s Puck lidar sensor [(previously VLP-16)](https://velodynelidar.com/products/puck/)
  + [Resources](https://velodynelidar.com/products/puck-lite/#downloads)
