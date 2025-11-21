#include "DCMTKTestInterface.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "cli/CommandRegistry.h"

#ifdef USE_DCMTK
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdicdir.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmjpeg/djdecode.h"
#include "dcmtk/dcmjpeg/djencode.h"

namespace fs = std::filesystem;

namespace {
std::string JoinPath(const std::string& base, const std::string& filename) {
    return (fs::path(base) / filename).string();
}
}

void DCMTKTests::RegisterCommands(CommandRegistry& registry) {
    registry.Register({
        "test-dcmtk",
        "DCMTK",
        "Run DCMTK feature tests",
        [](const CommandContext& ctx) {
            TestTagModification(ctx.inputPath, ctx.outputDir);
            TestPixelDataExtraction(ctx.inputPath, ctx.outputDir);
            TestLosslessJPEGReencode(ctx.inputPath, ctx.outputDir);
            TestRawDump(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "dcmtk:modify",
        "DCMTK",
        "Modify basic tags and persist a sanitized copy",
        [](const CommandContext& ctx) {
            TestTagModification(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "dcmtk:ppm",
        "DCMTK",
        "Export pixel data to portable map format",
        [](const CommandContext& ctx) {
            TestPixelDataExtraction(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "dcmtk:jpeg-lossless",
        "DCMTK",
        "Re-encode to JPEG Lossless to validate JPEG codec support",
        [](const CommandContext& ctx) {
            TestLosslessJPEGReencode(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "dcmtk:raw-dump",
        "DCMTK",
        "Dump raw pixel buffer for quick regression checks",
        [](const CommandContext& ctx) {
            TestRawDump(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });
}

void DCMTKTests::TestTagModification(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [DCMTK] Tag Modification ---" << std::endl;
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(filename.c_str());
    if (status.good()) {
        OFString patientName;
        if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good()) {
            std::cout << "Original Patient Name: " << patientName << std::endl;
        }

        std::cout << "Modifying PatientID to 'ANONYMIZED'..." << std::endl;
        fileformat.getDataset()->putAndInsertString(DCM_PatientID, "ANONYMIZED");

        std::string outFile = JoinPath(outputDir, "dcmtk_modified.dcm");
        status = fileformat.saveFile(outFile.c_str());
        if (status.good()) {
            std::cout << "Saved modified file to '" << outFile << "'" << std::endl;
        } else {
            std::cerr << "Error saving file: " << status.text() << std::endl;
        }
    } else {
        std::cerr << "Error reading file: " << status.text() << std::endl;
    }
}

void DCMTKTests::TestPixelDataExtraction(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [DCMTK] Pixel Data Extraction ---" << std::endl;
    
    DicomImage* image = new DicomImage(filename.c_str());
    if (image != NULL) {
        if (image->getStatus() == EIS_Normal) {
            std::cout << "Image loaded. Size: " << image->getWidth() << "x" << image->getHeight() << std::endl;
            
            if (image->isMonochrome()) {
                image->setMinMaxWindow();
            }

            std::string outFilename = JoinPath(outputDir, "dcmtk_pixel_output.ppm");
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

void DCMTKTests::TestDICOMDIRGeneration(const std::string& directory, const std::string& outputDir) {
    (void)directory;
    (void)outputDir;
    std::cout << "--- [DCMTK] DICOMDIR Generation ---" << std::endl;
    std::cout << "Note: Use dcmtk's dcmgpdir CLI for robust DICOMDIR creation." << std::endl;
}

void DCMTKTests::TestLosslessJPEGReencode(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [DCMTK] JPEG Lossless Re-encode ---" << std::endl;
    DJDecoderRegistration::registerCodecs();
    DJEncoderRegistration::registerCodecs();

    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(filename.c_str());
    if (!status.good()) {
        std::cerr << "Error reading file for JPEG re-encode: " << status.text() << std::endl;
        DJDecoderRegistration::cleanup();
        DJEncoderRegistration::cleanup();
        return;
    }

    std::string outFile = JoinPath(outputDir, "dcmtk_jpeg_lossless.dcm");
    status = fileformat.saveFile(outFile.c_str(), EXS_JPEGProcess14SV1);
    if (status.good()) {
        std::cout << "Saved JPEG Lossless file to '" << outFile << "'" << std::endl;
    } else {
        std::cerr << "JPEG re-encode failed: " << status.text() << std::endl;
    }

    DJDecoderRegistration::cleanup();
    DJEncoderRegistration::cleanup();
}

void DCMTKTests::TestRawDump(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [DCMTK] Raw Pixel Dump ---" << std::endl;
    DicomImage image(filename.c_str());
    if (image.getStatus() != EIS_Normal) {
        std::cerr << "Could not load image for raw dump: " << DicomImage::getString(image.getStatus()) << std::endl;
        return;
    }

    const int bits = image.isMonochrome() ? 16 : 24;
    const unsigned long count = image.getOutputDataSize(bits);
    if (count == 0) {
        std::cerr << "No pixel data available for raw dump." << std::endl;
        return;
    }

    std::vector<char> buffer(count);
    if (!image.getOutputData(buffer.data(), count, bits)) {
        std::cerr << "Failed to extract output data buffer." << std::endl;
        return;
    }

    std::string outFile = JoinPath(outputDir, "dcmtk_raw_dump.bin");
    std::ofstream out(outFile, std::ios::binary | std::ios::out | std::ios::trunc);
    out.write(buffer.data(), static_cast<std::streamsize>(count));
    if (out.good()) {
        std::cout << "Wrote raw buffer (" << count << " bytes) to " << outFile << std::endl;
    } else {
        std::cerr << "Failed writing raw buffer." << std::endl;
    }
}

#else
void DCMTKTests::RegisterCommands(CommandRegistry&) {}
void DCMTKTests::TestPixelDataExtraction(const std::string&, const std::string&) {}
void DCMTKTests::TestDICOMDIRGeneration(const std::string&, const std::string&) {}
void DCMTKTests::TestTagModification(const std::string&, const std::string&) { std::cout << "DCMTK not enabled." << std::endl; }
void DCMTKTests::TestLosslessJPEGReencode(const std::string&, const std::string&) {}
void DCMTKTests::TestRawDump(const std::string&, const std::string&) {}
#endif
