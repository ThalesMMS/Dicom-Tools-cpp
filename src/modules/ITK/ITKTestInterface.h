#pragma once
#include <string>

namespace ITKTests {
    void TestCannyEdgeDetection(const std::string& filename);
    void TestGaussianSmoothing(const std::string& filename);
    void TestResampling(const std::string& filename);
}
