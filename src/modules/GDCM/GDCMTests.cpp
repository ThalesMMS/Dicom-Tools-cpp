#include "GDCMTestInterface.h"
#include <iostream>

#ifdef USE_GDCM
#include "gdcmReader.h"
#include "gdcmWriter.h"
#include "gdcmImageReader.h"
#include "gdcmImageWriter.h"
#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmDefs.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"
#include "gdcmAnonymizer.h"
#include "gdcmImageChangeTransferSyntax.h"
#include "gdcmUIDs.h"

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
    std::cout << "--- [GDCM] Anonymization ---" << std::endl;
    
    gdcm::Reader reader;
    reader.SetFileName(filename.c_str());
    if (!reader.Read()) {
        std::cerr << "Could not read file for anonymization." << std::endl;
        return;
    }

    gdcm::Anonymizer anon;
    anon.SetFile(reader.GetFile());
    
    // Redact Patient Name
    anon.Empty(gdcm::Tag(0x0010, 0x0010));
    // Redact Patient ID
    anon.Empty(gdcm::Tag(0x0010, 0x0020));
    // Redact Birth Date
    anon.Empty(gdcm::Tag(0x0010, 0x0030));

    gdcm::Writer writer;
    std::string outFilename = "output/gdcm_anon.dcm";
    writer.SetFileName(outFilename.c_str());
    writer.SetFile(anon.GetFile());
    
    if (writer.Write()) {
        std::cout << "Anonymized file saved to: " << outFilename << std::endl;
    } else {
        std::cerr << "Failed to write anonymized file." << std::endl;
    }
}

void GDCMTests::TestDecompression(const std::string& filename) {
    std::cout << "--- [GDCM] Decompression (Transcoding to Raw) ---" << std::endl;
    
    gdcm::ImageChangeTransferSyntax change;
    change.SetTransferSyntax(gdcm::TransferSyntax::ImplicitVRLittleEndian);
    
    gdcm::ImageReader reader;
    reader.SetFileName(filename.c_str());
    if (!reader.Read()) {
        std::cerr << "Could not read file for decompression." << std::endl;
        return;
    }

    change.SetInput(reader.GetImage());
    if (!change.Change()) {
        std::cerr << "Could not change transfer syntax (decompression failed)." << std::endl;
        return;
    }

    gdcm::ImageWriter writer;
    std::string outFilename = "output/gdcm_raw.dcm";
    writer.SetFileName(outFilename.c_str());
    writer.SetFile(reader.GetFile());
    writer.SetImage(change.GetOutput());
    
    if (writer.Write()) {
        std::cout << "Decompressed file saved to: " << outFilename << std::endl;
    } else {
        std::cerr << "Failed to write decompressed file." << std::endl;
    }
}

#else
void GDCMTests::TestTagInspection(const std::string&) { std::cout << "GDCM not enabled." << std::endl; }
void GDCMTests::TestAnonymization(const std::string&) {}
void GDCMTests::TestDecompression(const std::string&) {}
#endif