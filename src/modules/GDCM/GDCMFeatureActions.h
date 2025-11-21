#pragma once

#include <string>

namespace GDCMTests {
    void TestTagInspection(const std::string& filename, const std::string& outputDir);
    void TestAnonymization(const std::string& filename, const std::string& outputDir);
    void TestDecompression(const std::string& filename, const std::string& outputDir);
    void TestUIDRewrite(const std::string& filename, const std::string& outputDir);
    void TestDatasetDump(const std::string& filename, const std::string& outputDir);
    void TestJPEG2000Transcode(const std::string& filename, const std::string& outputDir);
    void TestRLETranscode(const std::string& filename, const std::string& outputDir);
    void TestPixelStatistics(const std::string& filename, const std::string& outputDir);
}
