#include <iostream>
#include <array>
#include <algorithm>
#include "ousterOS1.hpp"
#include <cassert>
#include <cstring>

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

const static int lidarPort = 7502;
const static int imuPort = 7503;

int test();
int test2(OS1::LidarDataBlock& datablock);
int imuTest();
int float_test();

int main(int argc, char** argv) {
    test();
    float_test();
    imuTest();

    int sockfd;
    uint8_t buffer[48];

    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    memset(&servaddr, 0, sizeof(servaddr));
       
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(imuPort);

    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }

   
    while (true) {
        recvfrom(sockfd, buffer, 48,  
                MSG_WAITALL, nullptr, nullptr);

        OS1::ImuPacket imuPacket = OS1::ImuPacket(buffer);

        std::cout << std::dec << std::endl;
        std::cout << imuPacket.diagnostic_time << std::endl;
        std::cout << imuPacket.accelerometer_time << std::endl;
        std::cout << imuPacket.gyroscope_time << std::endl;
        std::cout << std::endl;
        std::cout << imuPacket.x_accel << std::endl;
        std::cout << imuPacket.y_accel << std::endl;
        std::cout << imuPacket.z_accel << std::endl;
        std::cout << std::endl;
        std::cout << imuPacket.x_rot << std::endl;
        std::cout << imuPacket.y_rot << std::endl;
        std::cout << imuPacket.z_rot << std::endl;
        std::cout << std::endl;
    }

    return 0;
}

int test() {
    uint8_t ldb_raw[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xBE, 0xCD, 0xAB, 0xFE, 0xC0, 0xEF, 0xBE};
    OS1::LidarDataBlock datablock = OS1::LidarDataBlock(ldb_raw);
    test2(datablock);

    uint8_t lcol_raw[788];

    uint64_t test_64 = 0xDEADBEEFC0FEBADD;
    uint32_t test_32 = 0xDEADBEEF;
    uint16_t test_16_1 = 0xBEEF;
    uint16_t test_16_2 = 0xC0FE;

    uint8_t* temp = lcol_raw;
    memcpy(temp    , &test_64  , sizeof(test_64)); // timestamp
    temp += 8;
    memcpy(temp  , &test_16_1, sizeof(test_16_1)); // measurement_id
    temp += 2;
    memcpy(temp  , &test_16_2, sizeof(test_16_2)); // frame_id
    temp += 2;
    memcpy(temp  , &test_32  , sizeof(test_32));   // encoder_count
    temp += 4;

    for (int i = 0; i < 64; i++) {
        memcpy(temp, &ldb_raw, sizeof(ldb_raw));
        temp += 12;
    }

    memcpy(lcol_raw+196*4, &test_32, sizeof(test_32));
    OS1::LidarCol lidarCol = OS1::LidarCol(lcol_raw);

    
    std::cout << std::hex << lidarCol.timestamp << std::endl;
    assert(lidarCol.timestamp       == test_64);
    std::cout << std::hex << lidarCol.measurement_id << std::endl;
    assert(lidarCol.measurement_id  == test_16_1);
    std::cout << std::hex << lidarCol.frame_id << std::endl;
    assert(lidarCol.frame_id        == test_16_2);
    std::cout << std::hex << lidarCol.encoder_count << std::endl;
    assert(lidarCol.encoder_count   == test_32);

    for (int i = 0;i < 64; i++) {
        test2(lidarCol.datablocks[i]);
    }

    std::cout << std::hex << lidarCol.block_status << std::endl;
    assert(lidarCol.block_status    == test_32);

    std::cout << std::endl << "LidarCol Passed" << std::endl << std::endl;

    return 1;
}

int test2(OS1::LidarDataBlock& datablock) {
    std::cout << std::hex << datablock.range << std::endl;
    assert(datablock.range          == 0x00FFFFF);
    std::cout << std::hex << datablock.reflectivity << std::endl;
    assert(datablock.reflectivity   == 0xBEEF);
    std::cout << std::hex << datablock.intensity << std::endl;
    assert(datablock.intensity      == 0xABCD);
    std::cout << std::hex << datablock.noise_photons << std::endl;
    assert(datablock.noise_photons  == 0xC0FE);

    std::cout << std::endl << "LidarBlock passed" << std::endl << std::endl;

    return 1;
}

int imuTest() {
    static_assert(sizeof(float) == 4, "float type is not 32bit");
    uint8_t raw[48];

    uint64_t test_64 = 0xDEADBEEFC0FEBADD;
    uint64_t test_64_2 = 0xBADDC0FEBEEFDEAD;

    uint32_t test_f[] = {0x4b3c614e}; // 12345678 in IEEE-754
    uint32_t test_f_2[] = {0x4ca72ff6}; // 87654320 in IEEE-754
    uint32_t test_f_3[] = {0xcb3c614e}; // -12345678 in IEE-754

    std::memcpy(raw, &test_64, sizeof(test_64));
    std::memcpy(raw+8, &test_64_2, sizeof(test_64_2));
    std::memcpy(raw+16, &test_64, sizeof(test_64));

    std::memcpy(raw+24, test_f, 4);
    std::memcpy(raw+28, test_f_2, 4);
    std::memcpy(raw+32, test_f_3, 4);

    std::memcpy(raw+36, test_f, 4);
    std::memcpy(raw+40, test_f_2, 4);
    std::memcpy(raw+44, test_f_3, 4);

    OS1::ImuPacket imuPacket = OS1::ImuPacket(raw);

    std::cout << imuPacket.diagnostic_time << std::endl;
    assert(imuPacket.diagnostic_time == test_64);
    std::cout << imuPacket.accelerometer_time << std::endl;
    assert(imuPacket.accelerometer_time == test_64_2);
    std::cout << imuPacket.gyroscope_time << std::endl;
    assert(imuPacket.gyroscope_time == test_64);

    std::cout << imuPacket.x_accel << std::endl;
    assert(imuPacket.x_accel ==  12345678);
    std::cout << imuPacket.y_accel << std::endl;
    assert(imuPacket.y_accel ==  87654320);
    std::cout << imuPacket.z_accel << std::endl;
    assert(imuPacket.z_accel == -12345678);

    std::cout << imuPacket.x_rot << std::endl;
    assert(imuPacket.x_rot ==  12345678);
    std::cout << imuPacket.y_rot << std::endl;
    assert(imuPacket.y_rot ==  87654320);
    std::cout << imuPacket.z_rot << std::endl;
    assert(imuPacket.z_rot == -12345678);

    std::cout << std::endl << "IMU Packet Passed" << std::endl << std::endl;
}

int float_test() {
    float temp = 12345678;

    uint8_t ans[] = {0x4e, 0x61, 0x3c, 0x4b};
    uint8_t raw[sizeof(temp)];

    std::memcpy(raw, &temp, sizeof(temp));

    std::cout << "0x";
    for (int i = 0; i < sizeof(temp); i++) {
        std::cout << std::hex << (unsigned) raw[i];
    }
    std::cout << std::endl  << std::endl;

    bool littleEndian = 1;
    for (int i = 0; i < sizeof(temp); i++) {
        littleEndian &= (raw[i] == ans[i]);
    }

    std::cout << ((littleEndian) ? "Little Endian Float" : "Big Endian Float") << std::endl << std::endl;
}