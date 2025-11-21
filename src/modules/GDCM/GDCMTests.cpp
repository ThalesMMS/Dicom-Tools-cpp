#include "GDCMTestInterface.h"

#include <filesystem>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "cli/CommandRegistry.h"

#ifdef USE_GDCM
#include "gdcmAnonymizer.h"
#include "gdcmAttribute.h"
#include "gdcmDefs.h"
#include "gdcmGlobal.h"
#include "gdcmImageChangeTransferSyntax.h"
#include "gdcmImageReader.h"
#include "gdcmImageWriter.h"
#include "gdcmReader.h"
#include "gdcmStringFilter.h"
#include "gdcmUIDs.h"
#include "gdcmWriter.h"
#include "gdcmPrinter.h"

namespace {
std::string JoinPath(const std::string& base, const std::string& name) {
    return (std::filesystem::path(base) / name).string();
}
}

void GDCMTests::RegisterCommands(CommandRegistry& registry) {
    registry.Register({
        "test-gdcm",
        "GDCM",
        "Run all GDCM feature tests",
        [](const CommandContext& ctx) {
            TestTagInspection(ctx.inputPath, ctx.outputDir);
            TestAnonymization(ctx.inputPath, ctx.outputDir);
            TestDecompression(ctx.inputPath, ctx.outputDir);
            TestUIDRewrite(ctx.inputPath, ctx.outputDir);
            TestDatasetDump(ctx.inputPath, ctx.outputDir);
            TestJPEG2000Transcode(ctx.inputPath, ctx.outputDir);
            TestRLETranscode(ctx.inputPath, ctx.outputDir);
            TestPixelStatistics(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "gdcm:tags",
        "GDCM",
        "Inspect common tags and print patient identifiers",
        [](const CommandContext& ctx) {
            TestTagInspection(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "gdcm:anonymize",
        "GDCM",
        "Strip PHI fields and write anonymized copy",
        [](const CommandContext& ctx) {
            TestAnonymization(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "gdcm:transcode-j2k",
        "GDCM",
        "Transcode to JPEG2000 (lossless) to validate codec support",
        [](const CommandContext& ctx) {
            TestJPEG2000Transcode(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "gdcm:retag-uids",
        "GDCM",
        "Regenerate Study/Series/SOP Instance UIDs and save copy",
        [](const CommandContext& ctx) {
            TestUIDRewrite(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "gdcm:dump",
        "GDCM",
        "Write a verbose dataset dump to text for QA",
        [](const CommandContext& ctx) {
            TestDatasetDump(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "gdcm:transcode-rle",
        "GDCM",
        "Transcode to RLE Lossless for encapsulated transfer syntax validation",
        [](const CommandContext& ctx) {
            TestRLETranscode(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "gdcm:stats",
        "GDCM",
        "Compute min/max/mean pixel stats and write to text",
        [](const CommandContext& ctx) {
            TestPixelStatistics(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });
}

void GDCMTests::TestTagInspection(const std::string& filename, const std::string& outputDir) {
    (void)outputDir;
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

    gdcm::Tag tagPatientName(0x0010, 0x0010);
    if (ds.FindDataElement(tagPatientName)) {
        std::cout << "Patient Name: " << sf.ToString(tagPatientName) << std::endl;
    } else {
        std::cout << "Patient Name: (Not Found)" << std::endl;
    }

    gdcm::Tag tagSOPInstanceUID(0x0008, 0x0018);
    if (ds.FindDataElement(tagSOPInstanceUID)) {
        std::cout << "SOP Instance UID: " << sf.ToString(tagSOPInstanceUID) << std::endl;
    }
}

void GDCMTests::TestAnonymization(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [GDCM] Anonymization ---" << std::endl;
    
    gdcm::Reader reader;
    reader.SetFileName(filename.c_str());
    if (!reader.Read()) {
        std::cerr << "Could not read file for anonymization." << std::endl;
        return;
    }

    gdcm::Anonymizer anon;
    anon.SetFile(reader.GetFile());
    
    anon.Empty(gdcm::Tag(0x0010, 0x0010));
    anon.Empty(gdcm::Tag(0x0010, 0x0020));
    anon.Empty(gdcm::Tag(0x0010, 0x0030));

    gdcm::Writer writer;
    std::string outFilename = JoinPath(outputDir, "gdcm_anon.dcm");
    writer.SetFileName(outFilename.c_str());
    writer.SetFile(anon.GetFile());
    
    if (writer.Write()) {
        std::cout << "Anonymized file saved to: " << outFilename << std::endl;
    } else {
        std::cerr << "Failed to write anonymized file." << std::endl;
    }
}

void GDCMTests::TestDecompression(const std::string& filename, const std::string& outputDir) {
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
    std::string outFilename = JoinPath(outputDir, "gdcm_raw.dcm");
    writer.SetFileName(outFilename.c_str());
    writer.SetFile(reader.GetFile());
    writer.SetImage(change.GetOutput());
    
    if (writer.Write()) {
        std::cout << "Decompressed file saved to: " << outFilename << std::endl;
    } else {
        std::cerr << "Failed to write decompressed file." << std::endl;
    }
}

void GDCMTests::TestUIDRewrite(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [GDCM] UID Regeneration ---" << std::endl;
    gdcm::Reader reader;
    reader.SetFileName(filename.c_str());
    if (!reader.Read()) {
        std::cerr << "Could not read file for UID rewrite." << std::endl;
        return;
    }

    gdcm::UIDGenerator uidGen;
    std::string studyUID = uidGen.Generate();
    std::string seriesUID = uidGen.Generate();
    std::string instanceUID = uidGen.Generate();

    auto setUID = [&](const gdcm::Tag& tag, const std::string& value) {
        gdcm::DataElement elem(tag);
        elem.SetByteValue(value.c_str(), static_cast<uint32_t>(value.size()));
        reader.GetFile().GetDataSet().Replace(elem);
    };

    setUID(gdcm::Tag(0x0020, 0x000D), studyUID);  // StudyInstanceUID
    setUID(gdcm::Tag(0x0020, 0x000E), seriesUID); // SeriesInstanceUID
    setUID(gdcm::Tag(0x0008, 0x0018), instanceUID); // SOPInstanceUID

    gdcm::Writer writer;
    std::string outFilename = JoinPath(outputDir, "gdcm_reuid.dcm");
    writer.SetFileName(outFilename.c_str());
    writer.SetFile(reader.GetFile());
    if (writer.Write()) {
        std::cout << "Assigned new Study/Series/SOP UIDs and saved to: " << outFilename << std::endl;
    } else {
        std::cerr << "Failed to write UID-regenerated file." << std::endl;
    }
}

void GDCMTests::TestDatasetDump(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [GDCM] Dataset Dump ---" << std::endl;
    gdcm::Reader reader;
    reader.SetFileName(filename.c_str());
    if (!reader.Read()) {
        std::cerr << "Could not read file for dataset dump." << std::endl;
        return;
    }

    std::string outFilename = JoinPath(outputDir, "gdcm_dump.txt");
    std::ofstream out(outFilename, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open output for dataset dump: " << outFilename << std::endl;
        return;
    }

    gdcm::Printer printer;
    printer.SetFile(reader.GetFile());
    printer.Print(out);
    std::cout << "Wrote verbose dataset dump to: " << outFilename << std::endl;
}

void GDCMTests::TestJPEG2000Transcode(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [GDCM] JPEG2000 Lossless Transcode ---" << std::endl;

    gdcm::ImageReader reader;
    reader.SetFileName(filename.c_str());
    if (!reader.Read()) {
        std::cerr << "Could not read file for JPEG2000 transcode." << std::endl;
        return;
    }

    gdcm::ImageChangeTransferSyntax change;
    change.SetTransferSyntax(gdcm::TransferSyntax::JPEG2000Lossless);
    change.SetInput(reader.GetImage());

    if (!change.Change()) {
        std::cerr << "Transfer syntax change to JPEG2000 failed (codec support may be missing)." << std::endl;
        return;
    }

    gdcm::ImageWriter writer;
    std::string outFilename = JoinPath(outputDir, "gdcm_jpeg2000.dcm");
    writer.SetFileName(outFilename.c_str());
    writer.SetFile(reader.GetFile());
    writer.SetImage(change.GetOutput());

    if (writer.Write()) {
        std::cout << "Transcoded to JPEG2000 and saved to: " << outFilename << std::endl;
    } else {
        std::cerr << "Failed to write JPEG2000 transcoded file." << std::endl;
    }
}

void GDCMTests::TestRLETranscode(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [GDCM] RLE Lossless Transcode ---" << std::endl;

    gdcm::ImageReader reader;
    reader.SetFileName(filename.c_str());
    if (!reader.Read()) {
        std::cerr << "Could not read file for RLE transcode." << std::endl;
        return;
    }

    gdcm::ImageChangeTransferSyntax change;
    change.SetTransferSyntax(gdcm::TransferSyntax::RLELossless);
    change.SetInput(reader.GetImage());

    if (!change.Change()) {
        std::cerr << "Transfer syntax change to RLE failed (codec support may be missing)." << std::endl;
        return;
    }

    gdcm::ImageWriter writer;
    std::string outFilename = JoinPath(outputDir, "gdcm_rle.dcm");
    writer.SetFileName(outFilename.c_str());
    writer.SetFile(reader.GetFile());
    writer.SetImage(change.GetOutput());

    if (writer.Write()) {
        std::cout << "Transcoded to RLE and saved to: " << outFilename << std::endl;
    } else {
        std::cerr << "Failed to write RLE transcoded file." << std::endl;
    }
}

namespace {
struct PixelStats {
    double min{0.0};
    double max{0.0};
    double mean{0.0};
    std::size_t count{0};
};

template <typename T>
PixelStats CalculateStats(const std::vector<char>& buffer) {
    PixelStats stats;
    const auto* data = reinterpret_cast<const T*>(buffer.data());
    const std::size_t count = buffer.size() / sizeof(T);
    if (count == 0) {
        return stats;
    }

    T minVal = std::numeric_limits<T>::max();
    T maxVal = std::numeric_limits<T>::lowest();
    long double sum = 0.0;
    for (std::size_t i = 0; i < count; ++i) {
        T value = data[i];
        minVal = std::min(minVal, value);
        maxVal = std::max(maxVal, value);
        sum += value;
    }

    stats.count = count;
    stats.min = static_cast<double>(minVal);
    stats.max = static_cast<double>(maxVal);
    stats.mean = static_cast<double>(sum / static_cast<long double>(count));
    return stats;
}
}

void GDCMTests::TestPixelStatistics(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [GDCM] Pixel Statistics ---" << std::endl;

    gdcm::ImageReader reader;
    reader.SetFileName(filename.c_str());
    if (!reader.Read()) {
        std::cerr << "Could not read file for statistics." << std::endl;
        return;
    }

    const gdcm::Image& image = reader.GetImage();
    const unsigned long bufferLength = image.GetBufferLength();
    if (bufferLength == 0) {
        std::cerr << "Image buffer length is zero." << std::endl;
        return;
    }

    std::vector<char> buffer(bufferLength);
    if (!image.GetBuffer(buffer.data())) {
        std::cerr << "Failed to read pixel buffer for statistics." << std::endl;
        return;
    }

    const gdcm::PixelFormat& pf = image.GetPixelFormat();
    PixelStats stats;
    bool supported = true;
    switch (pf.GetScalarType()) {
        case gdcm::PixelFormat::UINT8:
            stats = CalculateStats<uint8_t>(buffer);
            break;
        case gdcm::PixelFormat::INT8:
            stats = CalculateStats<int8_t>(buffer);
            break;
        case gdcm::PixelFormat::UINT16:
            stats = CalculateStats<uint16_t>(buffer);
            break;
        case gdcm::PixelFormat::INT16:
            stats = CalculateStats<int16_t>(buffer);
            break;
        default:
            supported = false;
            stats = CalculateStats<uint8_t>(buffer);
            break;
    }

    std::string outFilename = JoinPath(outputDir, "gdcm_stats.txt");
    std::ofstream out(outFilename, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open output for statistics: " << outFilename << std::endl;
        return;
    }

    out << "PixelCount=" << stats.count << "\n";
    out << "BitsAllocated=" << pf.GetBitsAllocated() << "\n";
    out << "SamplesPerPixel=" << pf.GetSamplesPerPixel() << "\n";
    out << "Min=" << stats.min << "\n";
    out << "Max=" << stats.max << "\n";
    out << "Mean=" << stats.mean << "\n";
    out << "ScalarTypeSupported=" << (supported ? "yes" : "fallback_uint8") << "\n";
    out.close();

    std::cout << "Wrote pixel statistics to: " << outFilename << std::endl;
}

#else
void GDCMTests::RegisterCommands(CommandRegistry&) {}
void GDCMTests::TestTagInspection(const std::string&, const std::string&) { std::cout << "GDCM not enabled." << std::endl; }
void GDCMTests::TestAnonymization(const std::string&, const std::string&) {}
void GDCMTests::TestDecompression(const std::string&, const std::string&) {}
void GDCMTests::TestUIDRewrite(const std::string&, const std::string&) {}
void GDCMTests::TestDatasetDump(const std::string&, const std::string&) {}
void GDCMTests::TestJPEG2000Transcode(const std::string&, const std::string&) {}
void GDCMTests::TestRLETranscode(const std::string&, const std::string&) {}
void GDCMTests::TestPixelStatistics(const std::string&, const std::string&) {}
#endif
