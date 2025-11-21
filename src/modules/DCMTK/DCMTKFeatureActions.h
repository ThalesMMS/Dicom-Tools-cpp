#pragma once

#include <string>

namespace DCMTKTests {
    void TestPixelDataExtraction(const std::string& filename, const std::string& outputDir);
    void TestDICOMDIRGeneration(const std::string& directory, const std::string& outputDir);
    void TestTagModification(const std::string& filename, const std::string& outputDir);
    void TestLosslessJPEGReencode(const std::string& filename, const std::string& outputDir);
    void TestRawDump(const std::string& filename, const std::string& outputDir);
    void TestExplicitVRRewrite(const std::string& filename, const std::string& outputDir);
    void TestMetadataReport(const std::string& filename, const std::string& outputDir);
    void TestRLEReencode(const std::string& filename, const std::string& outputDir);
    void TestJPEGBaseline(const std::string& filename, const std::string& outputDir);
    void TestBMPPreview(const std::string& filename, const std::string& outputDir);
}
