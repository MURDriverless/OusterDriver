#include "ousterOS1_util.hpp"
#include <cmath>

namespace OS1 {
    void LidarLUT::LoadTable(int W,
                             int H,
                             const std::vector<double>& azimuth_angles, 
                             const std::vector<double>& altitude_angles) {
        table = std::vector<double>(3 * W * H, 0);

        for (int colIdx = 0; colIdx < W; colIdx++) {
            double theta_0 = 2.0 * M_PI * colIdx / W;

            for (int vertIdx = 0; vertIdx < H; vertIdx++) {
                int idx = 3 * (colIdx * H + vertIdx);

                double theta = (azimuth_angles[idx] * 2 * M_PI / 360.0) + theta_0;

                // x LUT
                table[idx + 0] =  std::cos(altitude_angles[idx] * 2 * M_PI / 360.0) *
                                  std::cos(theta);
                // y LUT
                table[idx + 1] = -std::cos(altitude_angles[idx] * 2 * M_PI / 360.0) *
                                  std::sin(theta);

                // z LUT
                table[idx + 2] =  std::sin(altitude_angles[idx] * 2 * M_PI / 360.0);
            }
        }
    }
}