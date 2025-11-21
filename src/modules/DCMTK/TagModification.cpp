#include "DCMTKTestInterface.h"
#include <iostream>

#ifdef USE_DCMTK
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

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
        status = fileformat.saveFile("dcmtk_modified.dcm");
        if (status.good()) {
            std::cout << "Saved modified file to 'dcmtk_modified.dcm'" << std::endl;
        } else {
            std::cerr << "Error saving file: " << status.text() << std::endl;
        }
    } else {
        std::cerr << "Error reading file: " << status.text() << std::endl;
    }
}

void DCMTKTests::TestPixelDataExtraction(const std::string& filename) {
    std::cout << "--- [DCMTK] Pixel Data Extraction (Not Implemented) ---" << std::endl;
}

void DCMTKTests::TestDICOMDIRGeneration(const std::string& directory) {
    std::cout << "--- [DCMTK] DICOMDIR Generation (Not Implemented) ---" << std::endl;
}

#else
void DCMTKTests::TestTagModification(const std::string&) { std::cout << "DCMTK not enabled." << std::endl; }
void DCMTKTests::TestPixelDataExtraction(const std::string&) {}
void DCMTKTests::TestDICOMDIRGeneration(const std::string&) {}
#endif
