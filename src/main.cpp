#include <iostream>
#include <string>

#include "cli/CLIOptions.h"
#include "cli/CLIParser.h"
#include "cli/CommandRegistry.h"
#include "modules/DCMTK/DCMTKTestInterface.h"
#include "modules/GDCM/GDCMTestInterface.h"
#include "modules/ITK/ITKTestInterface.h"
#include "modules/VTK/VTKTestInterface.h"
#include "utils/FileSystemUtils.h"

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "      Dicom-Tools-cpp Command Suite     " << std::endl;
    std::cout << "========================================" << std::endl;

    CommandRegistry registry;
    GDCMTests::RegisterCommands(registry);
    DCMTKTests::RegisterCommands(registry);
    ITKTests::RegisterCommands(registry);
    VTKTests::RegisterCommands(registry);
    registry.Register({
        "all",
        "General",
        "Run every module suite",
        [&registry](const CommandContext& ctx) {
            int rc = 0;
            rc |= registry.Run("test-gdcm", ctx);
            rc |= registry.Run("test-dcmtk", ctx);
            rc |= registry.Run("test-itk", ctx);
            rc |= registry.Run("test-vtk", ctx);
            return rc;
        }
    });

    CLIOptions options = ParseCLIArgs(argc, argv, registry);

    if (options.list) {
        registry.List(std::cout);
        return 0;
    }

    if (options.help || options.command.empty()) {
        PrintUsage(std::cout, registry);
        return options.command.empty() ? 1 : 0;
    }

    if (!registry.Exists(options.command)) {
        std::cerr << "Unknown command: " << options.command << std::endl;
        PrintUsage(std::cout, registry);
        return 1;
    }

    std::string inputPath = options.inputPath;
    if (inputPath.empty()) {
        inputPath = FileSystemUtils::FindFirstDicom(INPUT_DIR);
        if (inputPath.empty()) {
            std::cerr << "Error: No .dcm file provided and none found in " << INPUT_DIR << std::endl;
            return 1;
        }
        std::cout << "Auto-detected input file: " << inputPath << std::endl;
    }

    if (!FileSystemUtils::EnsureOutputDir(options.outputDir)) {
        return 1;
    }

    CommandContext ctx{inputPath, options.outputDir, options.verbose};
    int result = registry.Run(options.command, ctx);

    std::cout << "========================================" << std::endl;
    return result;
}
