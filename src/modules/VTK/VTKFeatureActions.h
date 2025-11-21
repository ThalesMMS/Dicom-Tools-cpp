#pragma once

#include <string>

namespace VTKTests {
    void TestImageExport(const std::string& filename, const std::string& outputDir);
    void TestIsosurfaceExtraction(const std::string& filename, const std::string& outputDir);
    void TestMPR(const std::string& filename, const std::string& outputDir);
    void TestThresholdMask(const std::string& filename, const std::string& outputDir);
    void TestMetadataExport(const std::string& filename, const std::string& outputDir);
    void TestNiftiExport(const std::string& filename, const std::string& outputDir);
    void TestVolumeStatistics(const std::string& filename, const std::string& outputDir);
}
