#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <functional>

#include "modules/GDCM/GDCMTestInterface.h"
#include "modules/DCMTK/DCMTKTestInterface.h"
#include "modules/ITK/ITKTestInterface.h"
#include "modules/VTK/VTKTestInterface.h"

namespace fs = std::filesystem;

void PrintUsage() {
    std::cout << "Usage: ./DicomTools <command> [file_path]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  test-gdcm  : Run GDCM tests" << std::endl;
    std::cout << "  test-dcmtk : Run DCMTK tests" << std::endl;
    std::cout << "  test-itk   : Run ITK tests" << std::endl;
    std::cout << "  test-vtk   : Run VTK tests" << std::endl;
    std::cout << "  all        : Run all tests" << std::endl;
}

std::string FindFirstDicom(const std::string& inputDir) {
    if (fs::exists(inputDir) && fs::is_directory(inputDir)) {
        for (const auto& entry : fs::recursive_directory_iterator(inputDir)) {
            if (entry.path().extension() == ".dcm") {
                return entry.path().string();
            }
        }
    }
    return "";
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "      Dicom-Tools-cpp Test Runner       " << std::endl;
    std::cout << "========================================" << std::endl;

    if (argc < 2) {
        PrintUsage();
        return 1;
    }

    std::string command = argv[1];
    std::string filePath;

    if (argc > 2) {
        filePath = argv[2];
    } else {
        // Auto-find
        filePath = FindFirstDicom(INPUT_DIR);
        if (filePath.empty()) {
            std::cerr << "Error: No .dcm file provided and none found in " << INPUT_DIR << std::endl;
            return 1;
        }
        std::cout << "Auto-detected input file: " << filePath << std::endl;
    }

    if (command == "test-gdcm") {
        GDCMTests::TestTagInspection(filePath);
        GDCMTests::TestAnonymization(filePath);
        GDCMTests::TestDecompression(filePath);
    } else if (command == "test-dcmtk") {
        DCMTKTests::TestTagModification(filePath);
        DCMTKTests::TestPixelDataExtraction(filePath);
        DCMTKTests::TestDICOMDIRGeneration(filePath);
    } else if (command == "test-itk") {
        ITKTests::TestCannyEdgeDetection(filePath);
        ITKTests::TestGaussianSmoothing(filePath);
        ITKTests::TestResampling(filePath);
    } else if (command == "test-vtk") {
        VTKTests::TestImageExport(filePath);
        VTKTests::TestIsosurfaceExtraction(filePath);
        VTKTests::TestMPR(filePath);
    } else if (command == "all") {
        GDCMTests::TestTagInspection(filePath);
        DCMTKTests::TestTagModification(filePath);
        ITKTests::TestCannyEdgeDetection(filePath);
        VTKTests::TestImageExport(filePath);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        PrintUsage();
        return 1;
    }

    std::cout << "========================================" << std::endl;
    return 0;
}
