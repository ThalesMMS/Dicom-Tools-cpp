#pragma once
#include <string>

namespace GDCMTests {
    void TestTagInspection(const std::string& filename);
    void TestAnonymization(const std::string& filename);
    void TestDecompression(const std::string& filename);
}
