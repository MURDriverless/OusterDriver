#include <iostream>
#include <array>
#include <algorithm>
#include <cassert>
#include <cstring>

#include "ousterOS1.hpp"
#include "ousterOS1_tests.hpp"

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

const static int lidarPort = 7502;
const static int imuPort = 7503;

void imuListen();
void lidarListen();

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

}
