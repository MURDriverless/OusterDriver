#pragma once

#include <vector>

namespace OS1{
class LidarLUT {
    public:
        std::vector<double> table;

        static LidarLUT& getInstance() {
            static LidarLUT instance;
            return instance;
        }

        void LoadTable(int W,
                       int H,
                       const std::vector<double>& azimuth_angles,
                       const std::vector<double>& altitude_angles);

    private:
        LidarLUT() {}
};
} // End namespace OS1