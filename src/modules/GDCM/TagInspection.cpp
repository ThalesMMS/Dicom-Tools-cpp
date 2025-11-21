#include "GDCMTestInterface.h"
#include <iostream>

#ifdef USE_GDCM
#include "gdcmReader.h"
#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmDefs.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"

void GDCMTests::TestTagInspection(const std::string& filename) {
    std::cout << "--- [GDCM] Tag Inspection ---" << std::endl;
    gdcm::Reader reader;
    reader.SetFileName(filename.c_str());
    if (!reader.Read()) {
        std::cerr << "GDCM: Could not read file: " << filename << std::endl;
        return;
    }

    const gdcm::DataSet& ds = reader.GetFile().GetDataSet();
    gdcm::StringFilter sf;
    sf.SetFile(reader.GetFile());

    // Patient Name (0010,0010)
    gdcm::Tag tagPatientName(0x0010, 0x0010);
    if (ds.FindDataElement(tagPatientName)) {
        std::cout << "Patient Name: " << sf.ToString(tagPatientName) << std::endl;
    } else {
        std::cout << "Patient Name: (Not Found)" << std::endl;
    }

    // SOP Instance UID (0008,0018)
    gdcm::Tag tagSOPInstanceUID(0x0008, 0x0018);
    if (ds.FindDataElement(tagSOPInstanceUID)) {
        std::cout << "SOP Instance UID: " << sf.ToString(tagSOPInstanceUID) << std::endl;
    }
}

void GDCMTests::TestAnonymization(const std::string& filename) {
    std::cout << "--- [GDCM] Anonymization (Not Implemented) ---" << std::endl;
}

void GDCMTests::TestDecompression(const std::string& filename) {
    std::cout << "--- [GDCM] Decompression (Not Implemented) ---" << std::endl;
}

#else
void GDCMTests::TestTagInspection(const std::string&) { std::cout << "GDCM not enabled." << std::endl; }
void GDCMTests::TestAnonymization(const std::string&) {}
void GDCMTests::TestDecompression(const std::string&) {}
#endif
