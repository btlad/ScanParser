cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME ScanParser)
project(${PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# By default -- build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Project main executable source
add_executable(${PROJECT_NAME} main.cpp parser/parser.h parser/parser.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE parser)

# Add external packages
# ScanParser requires pcapplusplus, pcap and pthread libraries
target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/pcapplusplus)
target_link_directories(${PROJECT_NAME} PRIVATE /usr/lib/pcapplusplus)

# pcapplusplus libraries are static, so build  STATIC executable
set_target_properties(${PROJECT_NAME} PROPERTIES STATIC_LIBRARY_OPTIONS -static-libstdc++)
target_link_libraries(${PROJECT_NAME} -lPcap++ -lPacket++ -lCommon++ -lpcap -lpthread)

install(TARGETS ${PROJECT_NAME}
    DESTINATION bin
)
