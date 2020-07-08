#include <iostream>
#include <array>
#include <algorithm>
#include "ousterOS1.hpp"
#include <cassert>
#include <cstring>

int test();
int test2(OS1::LidarDataBlock& datablock);

int main(int argc, char** argv) {
    test();


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