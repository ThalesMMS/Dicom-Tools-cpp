#include "DCMTKTestInterface.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "cli/CommandRegistry.h"

#ifdef USE_DCMTK
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdicdir.h"
#include "dcmtk/dcmdata/dcddirif.h"
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
            TestExplicitVRRewrite(ctx.inputPath, ctx.outputDir);
            TestMetadataReport(ctx.inputPath, ctx.outputDir);
            TestDICOMDIRGeneration(ctx.inputPath, ctx.outputDir);
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

    registry.Register({
        "dcmtk:explicit-vr",
        "DCMTK",
        "Rewrite using Explicit VR Little Endian to validate transcoding",
        [](const CommandContext& ctx) {
            TestExplicitVRRewrite(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "dcmtk:metadata",
        "DCMTK",
        "Export common metadata fields to text",
        [](const CommandContext& ctx) {
            TestMetadataReport(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "dcmtk:dicomdir",
        "DCMTK",
        "Generate a simple DICOMDIR for the input series",
        [](const CommandContext& ctx) {
            TestDICOMDIRGeneration(ctx.inputPath, ctx.outputDir);
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
    std::cout << "--- [DCMTK] DICOMDIR Generation ---" << std::endl;
    fs::path sourceRoot = fs::is_directory(directory) ? fs::path(directory) : fs::path(directory).parent_path();
    fs::path mediaRoot = fs::path(outputDir) / "dicomdir_media";
    if (sourceRoot.empty() || !fs::exists(sourceRoot)) {
        std::cerr << "Input path is invalid for DICOMDIR generation." << std::endl;
        return;
    }

    std::vector<fs::path> dicomFiles;
    for (const auto& entry : fs::recursive_directory_iterator(sourceRoot)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dcm") {
            dicomFiles.push_back(entry.path());
        }
    }

    if (dicomFiles.empty()) {
        std::cerr << "No DICOM files found under " << sourceRoot << " to include in DICOMDIR." << std::endl;
        return;
    }

    std::error_code ec;
    fs::create_directories(mediaRoot, ec);
    if (ec) {
        std::cerr << "Failed to create media output root: " << mediaRoot << " (" << ec.message() << ")" << std::endl;
        return;
    }

    size_t copied = 0;
    for (const auto& dicom : dicomFiles) {
        ec.clear();
        fs::path relative = fs::relative(dicom, sourceRoot, ec);
        if (ec) {
            relative = dicom.filename();
        }
        fs::path dest = mediaRoot / relative;
        std::error_code mkdirEc;
        fs::create_directories(dest.parent_path(), mkdirEc);
        if (mkdirEc) {
            std::cerr << "Failed to create directory for " << dest << " (" << mkdirEc.message() << ")" << std::endl;
            continue;
        }
        std::error_code copyErr;
        fs::copy_file(dicom, dest, fs::copy_options::overwrite_existing, copyErr);
        if (!copyErr) {
            ++copied;
        } else {
            std::cerr << "Failed to copy " << dicom << " -> " << dest << " (" << copyErr.message() << ")" << std::endl;
        }
    }

    std::string dicomdirPath = (mediaRoot / "DICOMDIR").string();
    OFFilename dicomdirName(dicomdirPath.c_str());
    OFFilename rootDir(mediaRoot.c_str());
    DicomDirInterface dirif;
    dirif.disableConsistencyCheck(OFTrue);
    OFCondition status = dirif.createNewDicomDir(AP_GeneralPurpose, dicomdirName, "DICOMTOOLS");
    if (status.bad()) {
        std::cerr << "Failed to create DICOMDIR scaffold: " << status.text() << std::endl;
        return;
    }

    size_t added = 0;
    for (const auto& dicom : dicomFiles) {
        ec.clear();
        fs::path relative = fs::relative(dicom, sourceRoot, ec);
        if (ec) {
            relative = dicom.filename();
        }
        fs::path copiedPath = mediaRoot / relative;
        status = dirif.addDicomFile(OFFilename(copiedPath.c_str()), rootDir);
        if (status.good()) {
            ++added;
        } else {
            std::cerr << "  Skipped " << dicom << ": " << status.text() << std::endl;
        }
    }

    status = dirif.writeDicomDir();
    if (status.good()) {
        std::cout << "Copied " << copied << " files and wrote DICOMDIR (" << added << " entries) to '" << dicomdirPath << "'" << std::endl;
        std::cout << "Media root (relative references): " << mediaRoot << std::endl;
    } else {
        std::cerr << "Failed to write DICOMDIR: " << status.text() << std::endl;
    }
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

void DCMTKTests::TestExplicitVRRewrite(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [DCMTK] Explicit VR Little Endian ---" << std::endl;
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(filename.c_str());
    if (!status.good()) {
        std::cerr << "Error reading file for explicit VR rewrite: " << status.text() << std::endl;
        return;
    }

    std::string outFile = JoinPath(outputDir, "dcmtk_explicit_vr.dcm");
    status = fileformat.saveFile(outFile.c_str(), EXS_ExplicitVRLittleEndian);
    if (status.good()) {
        std::cout << "Saved Explicit VR Little Endian copy to '" << outFile << "'" << std::endl;
    } else {
        std::cerr << "Explicit VR transcode failed: " << status.text() << std::endl;
    }
}

void DCMTKTests::TestMetadataReport(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [DCMTK] Metadata Report ---" << std::endl;
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(filename.c_str());
    if (!status.good()) {
        std::cerr << "Error reading file for metadata report: " << status.text() << std::endl;
        return;
    }

    DcmDataset* dataset = fileformat.getDataset();
    std::string outFile = JoinPath(outputDir, "dcmtk_metadata.txt");
    std::ofstream out(outFile, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open metadata output: " << outFile << std::endl;
        return;
    }

    auto writeString = [&](const DcmTagKey& tag, const std::string& label) {
        OFString value;
        if (dataset->findAndGetOFString(tag, value).good()) {
            out << label << ": " << value << "\n";
        } else {
            out << label << ": (missing)\n";
        }
    };

    writeString(DCM_PatientName, "PatientName");
    writeString(DCM_PatientID, "PatientID");
    writeString(DCM_StudyInstanceUID, "StudyInstanceUID");
    writeString(DCM_SeriesInstanceUID, "SeriesInstanceUID");
    writeString(DCM_SOPInstanceUID, "SOPInstanceUID");
    writeString(DCM_Modality, "Modality");

    Uint16 rows = 0, cols = 0;
    if (dataset->findAndGetUint16(DCM_Rows, rows).good() && dataset->findAndGetUint16(DCM_Columns, cols).good()) {
        out << "Dimensions: " << cols << " x " << rows << "\n";
    }

    Sint32 frames = 0;
    if (dataset->findAndGetSint32(DCM_NumberOfFrames, frames).good()) {
        out << "NumberOfFrames: " << frames << "\n";
    }

    const E_TransferSyntax originalXfer = dataset->getCurrentXfer();
    DcmXfer xfer(originalXfer);
    out << "TransferSyntax: " << xfer.getXferName() << " (" << xfer.getXferID() << ")\n";

    out.close();
    std::cout << "Wrote metadata summary to '" << outFile << "'" << std::endl;
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
void DCMTKTests::TestExplicitVRRewrite(const std::string&, const std::string&) {}
void DCMTKTests::TestMetadataReport(const std::string&, const std::string&) {}
void DCMTKTests::TestRawDump(const std::string&, const std::string&) {}
#endif
