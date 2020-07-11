#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>
#include <cassert>
#include <cstring>

#include "ousterOS1.hpp"
#include "ousterOS1_tests.hpp"
#include "ousterOS1_util.hpp"

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

const static int lidarPort = 7502;
const static int imuPort = 7503;

void imuListen();
void lidarListen();

const std::vector<double> beam_altitude_angles = {
    16.611,  16.084,  15.557,  15.029,  14.502,  13.975,  13.447,  12.920,
    12.393,  11.865,  11.338,  10.811,  10.283,  9.756,   9.229,   8.701,
    8.174,   7.646,   7.119,   6.592,   6.064,   5.537,   5.010,   4.482,
    3.955,   3.428,   2.900,   2.373,   1.846,   1.318,   0.791,   0.264,
    -0.264,  -0.791,  -1.318,  -1.846,  -2.373,  -2.900,  -3.428,  -3.955,
    -4.482,  -5.010,  -5.537,  -6.064,  -6.592,  -7.119,  -7.646,  -8.174,
    -8.701,  -9.229,  -9.756,  -10.283, -10.811, -11.338, -11.865, -12.393,
    -12.920, -13.447, -13.975, -14.502, -15.029, -15.557, -16.084, -16.611,
};

const std::vector<double> beam_azimuth_angles = {
    3.164, 1.055, -1.055, -3.164, 3.164, 1.055, -1.055, -3.164,
    3.164, 1.055, -1.055, -3.164, 3.164, 1.055, -1.055, -3.164,
    3.164, 1.055, -1.055, -3.164, 3.164, 1.055, -1.055, -3.164,
    3.164, 1.055, -1.055, -3.164, 3.164, 1.055, -1.055, -3.164,
    3.164, 1.055, -1.055, -3.164, 3.164, 1.055, -1.055, -3.164,
    3.164, 1.055, -1.055, -3.164, 3.164, 1.055, -1.055, -3.164,
    3.164, 1.055, -1.055, -3.164, 3.164, 1.055, -1.055, -3.164,
    3.164, 1.055, -1.055, -3.164, 3.164, 1.055, -1.055, -3.164,
};

int main(int argc, char** argv) {
    lidarColTest();
    float_test();
    imuTest();

    lidarListen();

    return 0;
}

void imuListen() {
    int sockfd;
    uint8_t imuBuffer[OS1::IMU_PACKET_SIZE];

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
        recvfrom(sockfd, imuBuffer, sizeof(imuBuffer),  
                MSG_WAITALL, nullptr, nullptr);

        OS1::ImuPacket imuPacket = OS1::ImuPacket(imuBuffer);

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
}

void lidarListen() {
    int sockfd;
    uint8_t lidarBuffer[OS1::LIDAR_PACKET_SIZE];

    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    memset(&servaddr, 0, sizeof(servaddr));
       
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(lidarPort);

    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }

    int max = 0;
    int first = -1;

    OS1::LidarLUT lidarLUT = OS1::LidarLUT::getInstance();
    lidarLUT.LoadTable(2048, OS1::LIDAR_V_RES, beam_azimuth_angles, beam_altitude_angles);

    std::fstream outFile;
    std::ostringstream outString;
    outString.str("");
    outString.clear();

    int frameID = 0;
    char buf[100];
    outFile.open("./output/output_0.bin", std::ios::out);

    while (true) {
        recvfrom(sockfd, lidarBuffer, sizeof(lidarBuffer),  
                MSG_WAITALL, nullptr, nullptr);

        for (int i = 0; i < OS1::LIDAR_COL_PER_PACKET; i++) {
            OS1::LidarCol lidarCol = OS1::LidarCol(lidarBuffer + (i * OS1::LIDAR_COL_SIZE));

            // std::cout << std::dec << std::endl;
            // std::cout << lidarCol.measurement_id << std::endl;
            max = (lidarCol.measurement_id > max) ? lidarCol.measurement_id : max;

            if (first == lidarCol.measurement_id) {
                outFile << outString.str();
                std::cout << std::dec << max << std::endl;
                std::cout << "1 Frame" << std::endl;
                std::cout << outString.rdbuf() << std::endl;
                std::cout << std::endl;

                max = 0;

                outFile.flush();
                outFile.close();
                outString.str("");
                outString.clear();
                std::snprintf(buf, 100, "./output/output_%d.bin", frameID);
                outFile.open(buf, std::ios::out);

                frameID++;
                // goto done;
            }

            if (first == -1) {
                first = lidarCol.measurement_id;
            }

            for (int vertIdx = 0; vertIdx < OS1::LIDAR_V_RES; vertIdx++) {
                int idx = 3 * (lidarCol.measurement_id * OS1::LIDAR_V_RES + vertIdx);

                int x = lidarCol.datablocks[vertIdx].range * lidarLUT.table[idx+0];
                int y = lidarCol.datablocks[vertIdx].range * lidarLUT.table[idx+1];
                int z = lidarCol.datablocks[vertIdx].range * lidarLUT.table[idx+2];

                outString << x << "," << y << "," << z << std::endl;
            }
        }
    }

    done: 
    // outFile << outString.str();
    // outFile.close();
    return;
}
