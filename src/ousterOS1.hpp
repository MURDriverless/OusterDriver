#pragma once

#include <array>

/* 
DO NOT use struct packing/memcpy/std::copy to "shortcut" parsing of binary stream,
doing so is undefined behaviour. Further, when compiled on ARM chips may lead
to silent failures and segfaults when accessing misaligned memory.

Also Endianess would then be required to be checked per platform.
 */

namespace OS1 {
const int LIDAR_V_RES = 64;
const int LIDAR_COL_SIZE = 788;
const int LIDAR_DBLOCK_SIZE = 12;
const uint32_t INVALID_DATA_BLOCK = 0x0;
const uint32_t VALID_DATA_BLOCK = 0xFFFFFFFF;

// struct containing LiDAR datablock
class LidarDataBlock {
    public:
        // range in milimeters, discretized to the nearest 3mm
        uint32_t range:20;
        uint16_t reflectivity;
        uint16_t intensity;
        uint16_t noise_photons;

        LidarDataBlock() {}
        LidarDataBlock(const uint8_t* data);
};

// struct containg LiDAR vertical slice
struct LidarCol {
    public:
        // timestamp of the measurement (ns)
        uint64_t timestamp;
        // azimuth measurement (degrees)
        uint16_t measurement_id;
        // index of LiDAR scan
        uint16_t frame_id;
        // azimuth angle as encoder count (max = 90111)
        uint32_t encoder_count;
        // vertical slice datablocks
        std::array<LidarDataBlock, LIDAR_V_RES> datablocks;
        // valid if 0xFFFFFFFF, bad if 0x0
        uint32_t block_status;

        LidarCol(const uint8_t* data);
};

struct ImuPacket {
    public:
        uint64_t diagnostic_time;
        uint64_t accelerometer_time;
        uint64_t gyroscope_time;

        // acceleration in x axis (g)
        float x_accel;
        // acceleration in y axis (g)
        float y_accel;
        // acceleration in z axis (g)
        float z_accel;

        // angular velocity in x axis (deg/s)
        float x_rot;
        // angular velocity in y axis (deg/s)
        float y_rot;
        // angular velocity in z axis (deg/s)
        float z_rot;

        ImuPacket(const uint8_t* data);
};
} // End namespace OS1