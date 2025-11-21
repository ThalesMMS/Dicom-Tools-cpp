#include "VTKTestInterface.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "cli/CommandRegistry.h"

#ifdef USE_VTK
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkImageReslice.h"
#include "vtkImageShiftScale.h"
#include "vtkImageThreshold.h"
#include "vtkMarchingCubes.h"
#include "vtkMedicalImageProperties.h"
#include "vtkNew.h"
#include "vtkPNGWriter.h"
#include "vtkSmartPointer.h"
#include "vtkSTLWriter.h"
#include "vtkXMLImageDataWriter.h"

namespace fs = std::filesystem;

namespace {
std::string JoinPath(const std::string& base, const std::string& filename) {
    return (fs::path(base) / filename).string();
}

std::string ResolveSeriesDirectory(const std::string& path) {
    if (fs::is_directory(path)) {
        return path;
    }
    return fs::path(path).parent_path().string();
}
}

void VTKTests::RegisterCommands(CommandRegistry& registry) {
    registry.Register({
        "test-vtk",
        "VTK",
        "Run all VTK feature tests",
        [](const CommandContext& ctx) {
            TestImageExport(ctx.inputPath, ctx.outputDir);
            TestIsosurfaceExtraction(ctx.inputPath, ctx.outputDir);
            TestMPR(ctx.inputPath, ctx.outputDir);
            TestThresholdMask(ctx.inputPath, ctx.outputDir);
            TestMetadataExport(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "vtk:export",
        "VTK",
        "Convert to VTI volume",
        [](const CommandContext& ctx) {
            TestImageExport(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "vtk:isosurface",
        "VTK",
        "Generate STL mesh with marching cubes",
        [](const CommandContext& ctx) {
            TestIsosurfaceExtraction(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "vtk:mask",
        "VTK",
        "Binary threshold to create a segmentation mask",
        [](const CommandContext& ctx) {
            TestThresholdMask(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "vtk:metadata",
        "VTK",
        "Export patient/study metadata to text",
        [](const CommandContext& ctx) {
            TestMetadataExport(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });
}

void VTKTests::TestImageExport(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [VTK] Image Export ---" << std::endl;

    vtkNew<vtkDICOMImageReader> reader;
    reader->SetFileName(filename.c_str());
    reader->Update();

    int* dims = reader->GetOutput()->GetDimensions();
    std::cout << "Dimensions: " << dims[0] << " x " << dims[1] << " x " << dims[2] << std::endl;

    vtkNew<vtkXMLImageDataWriter> writer;
    writer->SetFileName(JoinPath(outputDir, "vtk_export.vti").c_str());
    writer->SetInputData(reader->GetOutput());
    writer->Write();
    std::cout << "Saved to '" << writer->GetFileName() << "'" << std::endl;
}

void VTKTests::TestIsosurfaceExtraction(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [VTK] Isosurface Extraction (Marching Cubes) ---" << std::endl;
    
    vtkNew<vtkDICOMImageReader> reader;
    reader->SetDirectoryName(ResolveSeriesDirectory(filename).c_str());
    reader->Update();
    
    vtkNew<vtkMarchingCubes> surface;
    surface->SetInputConnection(reader->GetOutputPort());
    surface->ComputeNormalsOn();
    surface->ComputeGradientsOn();
    surface->SetValue(0, 500);

    vtkNew<vtkSTLWriter> writer;
    writer->SetFileName(JoinPath(outputDir, "vtk_isosurface.stl").c_str());
    writer->SetInputConnection(surface->GetOutputPort());
    writer->Write();
    
    std::cout << "Saved to '" << writer->GetFileName() << "'" << std::endl;
}

void VTKTests::TestMPR(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [VTK] MPR (Single Slice Export) ---" << std::endl;
    
    vtkNew<vtkDICOMImageReader> reader;
    reader->SetDirectoryName(ResolveSeriesDirectory(filename).c_str());
    reader->Update();
    
    double* center = reader->GetOutput()->GetCenter();
    double* range = reader->GetOutput()->GetScalarRange();
    
    vtkNew<vtkImageReslice> reslice;
    reslice->SetInputConnection(reader->GetOutputPort());
    reslice->SetOutputDimensionality(2);
    reslice->SetResliceAxesOrigin(center[0], center[1], center[2]);
    
    vtkNew<vtkImageShiftScale> shiftScale;
    shiftScale->SetInputConnection(reslice->GetOutputPort());
    shiftScale->SetShift(-range[0]);
    shiftScale->SetScale(255.0 / (range[1] - range[0]));
    shiftScale->SetOutputScalarTypeToUnsignedChar();
    
    vtkNew<vtkPNGWriter> writer;
    writer->SetFileName(JoinPath(outputDir, "vtk_mpr_slice.png").c_str());
    writer->SetInputConnection(shiftScale->GetOutputPort());
    writer->Write();
    
    std::cout << "Saved to '" << writer->GetFileName() << "'" << std::endl;
}

void VTKTests::TestThresholdMask(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [VTK] Threshold Mask ---" << std::endl;

    vtkNew<vtkDICOMImageReader> reader;
    reader->SetDirectoryName(ResolveSeriesDirectory(filename).c_str());
    reader->Update();

    vtkNew<vtkImageThreshold> threshold;
    threshold->SetInputConnection(reader->GetOutputPort());
    threshold->ThresholdBetween(300, 3000);
    threshold->SetInValue(1);
    threshold->SetOutValue(0);
    threshold->SetOutputScalarTypeToUnsignedChar();

    vtkNew<vtkXMLImageDataWriter> writer;
    writer->SetFileName(JoinPath(outputDir, "vtk_threshold_mask.vti").c_str());
    writer->SetInputConnection(threshold->GetOutputPort());
    writer->Write();

    std::cout << "Saved binary mask to '" << writer->GetFileName() << "'" << std::endl;
}

void VTKTests::TestMetadataExport(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [VTK] Metadata Export ---" << std::endl;

    vtkNew<vtkDICOMImageReader> reader;
    reader->SetDirectoryName(ResolveSeriesDirectory(filename).c_str());
    reader->Update();

    vtkMedicalImageProperties* props = reader->GetMedicalImageProperties();
    if (!props) {
        std::cerr << "No medical image properties available for metadata export." << std::endl;
        return;
    }
    std::string outFile = JoinPath(outputDir, "vtk_metadata.txt");
    std::ofstream out(outFile, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open metadata output: " << outFile << std::endl;
        return;
    }

    out << "PatientName: " << props->GetPatientName() << "\n";
    out << "PatientID: " << props->GetPatientID() << "\n";
    out << "StudyInstanceUID: " << props->GetStudyID() << "\n";
    out << "SeriesDescription: " << props->GetSeriesDescription() << "\n";
    out << "Modality: " << props->GetModality() << "\n";
    out << "InstitutionName: " << props->GetInstitutionName() << "\n";
    out.close();

    std::cout << "Wrote metadata summary to '" << outFile << "'" << std::endl;
}

#else
void VTKTests::RegisterCommands(CommandRegistry&) {}
void VTKTests::TestImageExport(const std::string&, const std::string&) { std::cout << "VTK not enabled." << std::endl; }
void VTKTests::TestIsosurfaceExtraction(const std::string&, const std::string&) {}
void VTKTests::TestMPR(const std::string&, const std::string&) {}
void VTKTests::TestThresholdMask(const std::string&, const std::string&) {}
void VTKTests::TestMetadataExport(const std::string&, const std::string&) {}
#endif
