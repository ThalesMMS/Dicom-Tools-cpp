#pragma once

#include <string>

namespace ITKTests {
    void TestCannyEdgeDetection(const std::string& filename, const std::string& outputDir);
    void TestGaussianSmoothing(const std::string& filename, const std::string& outputDir);
    void TestBinaryThresholding(const std::string& filename, const std::string& outputDir);
    void TestResampling(const std::string& filename, const std::string& outputDir);
    void TestAdaptiveHistogram(const std::string& filename, const std::string& outputDir);
    void TestSliceExtraction(const std::string& filename, const std::string& outputDir);
    void TestMedianFilter(const std::string& filename, const std::string& outputDir);
    void TestNRRDExport(const std::string& filename, const std::string& outputDir);
    void TestOtsuSegmentation(const std::string& filename, const std::string& outputDir);
    void TestAnisotropicDenoise(const std::string& filename, const std::string& outputDir);
    void TestMaximumIntensityProjection(const std::string& filename, const std::string& outputDir);
    void TestNiftiExport(const std::string& filename, const std::string& outputDir);
}
