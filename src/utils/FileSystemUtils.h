#pragma once

#include <string>

namespace FileSystemUtils {
    std::string FindFirstDicom(const std::string& inputDir);
    bool EnsureOutputDir(const std::string& path);
}
