#pragma once
#include <string>

class CommandRegistry;

namespace VTKTests {
    void RegisterCommands(CommandRegistry& registry);

    void TestImageExport(const std::string& filename, const std::string& outputDir);
    void TestIsosurfaceExtraction(const std::string& filename, const std::string& outputDir);
    void TestMPR(const std::string& filename, const std::string& outputDir);
    void TestThresholdMask(const std::string& filename, const std::string& outputDir);
    void TestMetadataExport(const std::string& filename, const std::string& outputDir);
}
