#pragma once
#include <string>

namespace DCMTKTests {
    void TestPixelDataExtraction(const std::string& filename);
    void TestDICOMDIRGeneration(const std::string& directory);
    void TestTagModification(const std::string& filename);
}
