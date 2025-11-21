#include "VTKTestInterface.h"
#include <iostream>
#include <filesystem>

#ifdef USE_VTK
#include "vtkSmartPointer.h"
#include "vtkDICOMImageReader.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkImageData.h"
#include "vtkNew.h"
#include "vtkMarchingCubes.h"
#include "vtkSTLWriter.h"
#include "vtkImageReslice.h"
#include "vtkPNGWriter.h"
#include "vtkMatrix4x4.h"

namespace fs = std::filesystem;

void VTKTests::TestImageExport(const std::string& filename) {
    std::cout << "--- [VTK] Image Export ---" << std::endl;

    vtkNew<vtkDICOMImageReader> reader;
    // If filename is a file, we might need to set Directory Name instead for full series
    // But for single file test, we use SetFileName
    reader->SetFileName(filename.c_str());
    reader->Update();

    int* dims = reader->GetOutput()->GetDimensions();
    std::cout << "Dimensions: " << dims[0] << " x " << dims[1] << " x " << dims[2] << std::endl;

    vtkNew<vtkXMLImageDataWriter> writer;
    writer->SetFileName("output/vtk_export.vti");
    writer->SetInputData(reader->GetOutput());
    writer->Write();
    std::cout << "Saved to 'output/vtk_export.vti'" << std::endl;
}

void VTKTests::TestIsosurfaceExtraction(const std::string& filename) {
    std::cout << "--- [VTK] Isosurface Extraction (Marching Cubes) ---" << std::endl;
    
    vtkNew<vtkDICOMImageReader> reader;
    
    // Heuristic: if filename is a directory, use SetDirectoryName
    if (fs::is_directory(filename)) {
        reader->SetDirectoryName(filename.c_str());
    } else {
        // Try to read just the file, or the parent directory? 
        // Usually isosurface needs a volume. Let's try reading the single file as a volume of 1 slice (will probably fail to generate nice mesh)
        // Or assume the user passed one file of a series and we want the series.
        fs::path p(filename);
        reader->SetDirectoryName(p.parent_path().string().c_str());
    }
    reader->Update();
    
    vtkNew<vtkMarchingCubes> surface;
    surface->SetInputConnection(reader->GetOutputPort());
    surface->ComputeNormalsOn();
    surface->ComputeGradientsOn();
    surface->SetValue(0, 500); // Arbitrary iso-value for bone/skin

    vtkNew<vtkSTLWriter> writer;
    writer->SetFileName("output/vtk_isosurface.stl");
    writer->SetInputConnection(surface->GetOutputPort());
    writer->Write();
    
    std::cout << "Saved to 'output/vtk_isosurface.stl'" << std::endl;
}

#include "vtkImageShiftScale.h"

void VTKTests::TestMPR(const std::string& filename) {
    std::cout << "--- [VTK] MPR (Single Slice Export) ---" << std::endl;
    
    vtkNew<vtkDICOMImageReader> reader;
    if (fs::is_directory(filename)) {
        reader->SetDirectoryName(filename.c_str());
    } else {
        fs::path p(filename);
        reader->SetDirectoryName(p.parent_path().string().c_str());
    }
    reader->Update();
    
    // Get center
    double* center = reader->GetOutput()->GetCenter();
    double* range = reader->GetOutput()->GetScalarRange();
    
    vtkNew<vtkImageReslice> reslice;
    reslice->SetInputConnection(reader->GetOutputPort());
    reslice->SetOutputDimensionality(2);
    // Default is axial, let's just extract the center slice
    reslice->SetResliceAxesOrigin(center[0], center[1], center[2]);
    
    // Cast to Unsigned Char (0-255) for PNG
    vtkNew<vtkImageShiftScale> shiftScale;
    shiftScale->SetInputConnection(reslice->GetOutputPort());
    shiftScale->SetShift(-range[0]);
    shiftScale->SetScale(255.0 / (range[1] - range[0]));
    shiftScale->SetOutputScalarTypeToUnsignedChar();
    
    vtkNew<vtkPNGWriter> writer;
    writer->SetFileName("output/vtk_mpr_slice.png");
    writer->SetInputConnection(shiftScale->GetOutputPort());
    writer->Write();
    
    std::cout << "Saved to 'output/vtk_mpr_slice.png'" << std::endl;
}

#else
void VTKTests::TestImageExport(const std::string&) { std::cout << "VTK not enabled." << std::endl; }
void VTKTests::TestIsosurfaceExtraction(const std::string&) {}
void VTKTests::TestMPR(const std::string&) {}
#endif