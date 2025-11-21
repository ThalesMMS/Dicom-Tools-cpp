#pragma once
#include <string>

class CommandRegistry;

namespace DCMTKTests {
    void RegisterCommands(CommandRegistry& registry);

    void TestPixelDataExtraction(const std::string& filename, const std::string& outputDir);
    void TestDICOMDIRGeneration(const std::string& directory, const std::string& outputDir);
    void TestTagModification(const std::string& filename, const std::string& outputDir);
    void TestLosslessJPEGReencode(const std::string& filename, const std::string& outputDir);
    void TestRawDump(const std::string& filename, const std::string& outputDir);
}
