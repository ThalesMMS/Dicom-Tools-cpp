#include "ITKTestInterface.h"

#include "ITKFeatureActions.h"
#include "cli/CommandRegistry.h"

#ifdef USE_ITK

void ITKTests::RegisterCommands(CommandRegistry& registry) {
    registry.Register({
        "test-itk",
        "ITK",
        "Run all ITK feature tests",
        [](const CommandContext& ctx) {
            TestCannyEdgeDetection(ctx.inputPath, ctx.outputDir);
            TestGaussianSmoothing(ctx.inputPath, ctx.outputDir);
            TestMedianFilter(ctx.inputPath, ctx.outputDir);
            TestBinaryThresholding(ctx.inputPath, ctx.outputDir);
            TestResampling(ctx.inputPath, ctx.outputDir);
            TestAdaptiveHistogram(ctx.inputPath, ctx.outputDir);
            TestSliceExtraction(ctx.inputPath, ctx.outputDir);
            TestNRRDExport(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "itk:canny",
        "ITK",
        "Run 3D canny edge detection and write DICOM",
        [](const CommandContext& ctx) {
            TestCannyEdgeDetection(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "itk:histogram",
        "ITK",
        "Adaptive histogram equalization for contrast boost",
        [](const CommandContext& ctx) {
            TestAdaptiveHistogram(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "itk:slice",
        "ITK",
        "Extract middle axial slice to PNG",
        [](const CommandContext& ctx) {
            TestSliceExtraction(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "itk:median",
        "ITK",
        "Median smoothing for salt-and-pepper noise removal",
        [](const CommandContext& ctx) {
            TestMedianFilter(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });

    registry.Register({
        "itk:nrrd",
        "ITK",
        "Export the volume to NRRD for interchange",
        [](const CommandContext& ctx) {
            TestNRRDExport(ctx.inputPath, ctx.outputDir);
            return 0;
        }
    });
}

#else
void ITKTests::RegisterCommands(CommandRegistry&) {}
#endif
