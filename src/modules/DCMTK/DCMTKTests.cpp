#include "DCMTKTestInterface.h"
#include <iostream>
#include <filesystem>

#ifdef USE_DCMTK
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcdicdir.h"
#include "dcmtk/dcmimgle/dcmimage.h"

namespace fs = std::filesystem;

void DCMTKTests::TestTagModification(const std::string& filename) {
    std::cout << "--- [DCMTK] Tag Modification ---" << std::endl;
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(filename.c_str());
    if (status.good()) {
        OFString patientName;
        if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good()) {
            std::cout << "Original Patient Name: " << patientName << std::endl;
        }

        // Modify PatientID
        std::cout << "Modifying PatientID to 'ANONYMIZED'..." << std::endl;
        fileformat.getDataset()->putAndInsertString(DCM_PatientID, "ANONYMIZED");

        // Save
        status = fileformat.saveFile("output/dcmtk_modified.dcm");
        if (status.good()) {
            std::cout << "Saved modified file to 'output/dcmtk_modified.dcm'" << std::endl;
        } else {
            std::cerr << "Error saving file: " << status.text() << std::endl;
        }
    } else {
        std::cerr << "Error reading file: " << status.text() << std::endl;
    }
}

void DCMTKTests::TestPixelDataExtraction(const std::string& filename) {
    std::cout << "--- [DCMTK] Pixel Data Extraction ---" << std::endl;
    
    DicomImage* image = new DicomImage(filename.c_str());
    if (image != NULL) {
        if (image->getStatus() == EIS_Normal) {
            std::cout << "Image loaded. Size: " << image->getWidth() << "x" << image->getHeight() << std::endl;
            
            // Set window/level to default if present
            if (image->isMonochrome()) {
                image->setMinMaxWindow();
            }

            std::string outFilename = "output/dcmtk_pixel_output.ppm";
            // writePPM writes PGM for monochrome, PPM for color
            if (image->writePPM(outFilename.c_str())) {
                 std::cout << "Saved PPM/PGM image to: " << outFilename << std::endl;
            } else {
                std::cerr << "Failed to write PPM image." << std::endl;
            }
        } else {
            std::cerr << "Error: cannot load DICOM image (" << DicomImage::getString(image->getStatus()) << ")" << std::endl;
        }
        delete image;
    } else {
        std::cerr << "Error: Memory allocation failed for DicomImage." << std::endl;
    }
}

void DCMTKTests::TestDICOMDIRGeneration(const std::string& directory) {
    std::cout << "--- [DCMTK] DICOMDIR Generation ---" << std::endl;
    std::cout << "Skipped: Programmatic DICOMDIR generation using raw DCMTK classes is complex and beyond this simple demo." << std::endl;
    std::cout << "Please use the 'dcmgpdir' CLI tool provided by DCMTK for this purpose." << std::endl;
}

#else
void DCMTKTests::TestTagModification(const std::string&) { std::cout << "DCMTK not enabled." << std::endl; }
void DCMTKTests::TestPixelDataExtraction(const std::string&) {}
void DCMTKTests::TestDICOMDIRGeneration(const std::string&) {}
#endif
