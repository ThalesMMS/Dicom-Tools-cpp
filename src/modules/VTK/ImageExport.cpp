#include "VTKTestInterface.h"
#include <iostream>

#ifdef USE_VTK
#include "vtkSmartPointer.h"
#include "vtkDICOMImageReader.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkImageData.h"
#include "vtkNew.h"

void VTKTests::TestImageExport(const std::string& filename) {
    std::cout << "--- [VTK] Image Export ---" << std::endl;

    vtkNew<vtkDICOMImageReader> reader;
    reader->SetFileName(filename.c_str());
    reader->Update();

    int* dims = reader->GetOutput()->GetDimensions();
    std::cout << "Dimensions: " << dims[0] << " x " << dims[1] << " x " << dims[2] << std::endl;

    vtkNew<vtkXMLImageDataWriter> writer;
    writer->SetFileName("vtk_export.vti");
    writer->SetInputData(reader->GetOutput());
    writer->Write();
    std::cout << "Saved to 'vtk_export.vti'" << std::endl;
}

void VTKTests::TestIsosurfaceExtraction(const std::string& filename) {
    std::cout << "--- [VTK] Isosurface Extraction (Not Implemented) ---" << std::endl;
}

void VTKTests::TestMPR(const std::string& filename) {
    std::cout << "--- [VTK] MPR (Not Implemented) ---" << std::endl;
}

#else
void VTKTests::TestImageExport(const std::string&) { std::cout << "VTK not enabled." << std::endl; }
void VTKTests::TestIsosurfaceExtraction(const std::string&) {}
void VTKTests::TestMPR(const std::string&) {}
#endif
