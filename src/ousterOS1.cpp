#include "ousterOS1.hpp"
#include <iostream>
#include <array>
#include <algorithm>

/*
Parse binary stream as per
https://data.ouster.io/downloads/software-user-guide-v1.13.0.pdf
*/

namespace OS1 {
LidarDataBlock::LidarDataBlock(const uint8_t* data) {
    range  = ((uint32_t) data[0]);
    range |= ((uint32_t) data[1]) << 8;
    range |= ((uint32_t) data[2]) << 16;
    range |= ((uint32_t) data[3]) << 24;

    reflectivity  = ((uint16_t) data[4]);
    reflectivity |= ((uint16_t) data[5]) << 8;

    intensity  = ((uint16_t) data[6]);
    intensity |= ((uint16_t) data[7]) << 8;

    noise_photons  = ((uint16_t) data[8]);
    noise_photons |= ((uint16_t) data[9]) << 8;
}

LidarCol::LidarCol(const uint8_t* data) {
    timestamp = ((uint64_t) data[0]);
    for (int i = 1; i < 8; i++) {
        timestamp |= ((uint64_t) data[i]) << (8*i);
    }

    measurement_id  = ((uint16_t) data[8]);
    measurement_id |= ((uint16_t) data[9]) << 8;

    frame_id  = ((uint16_t) data[10]);
    frame_id |= ((uint16_t) data[11]) << 8;

    encoder_count  = ((uint32_t) data[12]);
    encoder_count |= ((uint32_t) data[13]) << 8;
    encoder_count |= ((uint32_t) data[14]) << 16;
    encoder_count |= ((uint32_t) data[15]) << 24;

    for (int i = 0; i < 64; i++) {
        datablocks[i] = LidarDataBlock(data + 16 + i*LIDAR_DBLOCK_SIZE);
    }

    block_status  = ((uint32_t) data[784]);
    block_status |= ((uint32_t) data[785]) << 8;
    block_status |= ((uint32_t) data[786]) << 16;
    block_status |= ((uint32_t) data[787]) << 24;
}

} // End namespace OS1