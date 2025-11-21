#include "ITKTestInterface.h"

#include <filesystem>
#include <iostream>

#include "cli/CommandRegistry.h"

#ifdef USE_ITK
#include "itkAdaptiveHistogramEqualizationImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkIdentityTransform.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkPNGImageIO.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

namespace {
std::string JoinPath(const std::string& base, const std::string& filename) {
    return (std::filesystem::path(base) / filename).string();
}
}

void ITKTests::RegisterCommands(CommandRegistry& registry) {
    registry.Register({
        "test-itk",
        "ITK",
        "Run all ITK feature tests",
        [](const CommandContext& ctx) {
            TestCannyEdgeDetection(ctx.inputPath, ctx.outputDir);
            TestGaussianSmoothing(ctx.inputPath, ctx.outputDir);
            TestBinaryThresholding(ctx.inputPath, ctx.outputDir);
            TestResampling(ctx.inputPath, ctx.outputDir);
            TestAdaptiveHistogram(ctx.inputPath, ctx.outputDir);
            TestSliceExtraction(ctx.inputPath, ctx.outputDir);
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
}

void ITKTests::TestCannyEdgeDetection(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [ITK] Canny Edge Detection ---" << std::endl;
    
    using InputPixelType = float;
    using OutputPixelType = unsigned char;
    const unsigned int Dimension = 3;
    
    using InputImageType = itk::Image<InputPixelType, Dimension>;
    using OutputImageType = itk::Image<OutputPixelType, Dimension>;
    
    using ReaderType = itk::ImageFileReader<InputImageType>;
    using WriterType = itk::ImageFileWriter<OutputImageType>;
    using FilterType = itk::CannyEdgeDetectionImageFilter<InputImageType, InputImageType>;
    using RescaleType = itk::RescaleIntensityImageFilter<InputImageType, OutputImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    
    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    reader->SetImageIO(gdcmIO);

    try {
        reader->Update();
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Exception: " << err << std::endl;
        return;
    }

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(reader->GetOutput());
    filter->SetVariance(2.0);
    filter->SetUpperThreshold(0.05);
    filter->SetLowerThreshold(0.02);
    
    RescaleType::Pointer rescaler = RescaleType::New();
    rescaler->SetInput(filter->GetOutput());
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(JoinPath(outputDir, "itk_canny.dcm"));
    writer->SetInput(rescaler->GetOutput());
    writer->SetImageIO(gdcmIO);

    try {
        writer->Update();
        std::cout << "Saved to '" << writer->GetFileName() << "'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

void ITKTests::TestGaussianSmoothing(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [ITK] Gaussian Smoothing ---" << std::endl;
    
    using PixelType = signed short;
    const unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;
    using ReaderType = itk::ImageFileReader<ImageType>;
    using WriterType = itk::ImageFileWriter<ImageType>;
    using FilterType = itk::DiscreteGaussianImageFilter<ImageType, ImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    
    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    reader->SetImageIO(gdcmIO);

    try {
        reader->Update();
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Exception: " << err << std::endl;
        return;
    }

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(reader->GetOutput());
    filter->SetVariance(1.0);

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(JoinPath(outputDir, "itk_gaussian.dcm"));
    writer->SetInput(filter->GetOutput());
    writer->SetImageIO(gdcmIO);

    try {
        writer->Update();
        std::cout << "Saved to '" << writer->GetFileName() << "'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

void ITKTests::TestBinaryThresholding(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [ITK] Binary Thresholding ---" << std::endl;
    
    using PixelType = signed short;
    const unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;
    using ReaderType = itk::ImageFileReader<ImageType>;
    using WriterType = itk::ImageFileWriter<ImageType>;
    using FilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    
    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    reader->SetImageIO(gdcmIO);

    try {
        reader->Update();
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Exception: " << err << std::endl;
        return;
    }

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(reader->GetOutput());
    filter->SetLowerThreshold(200);
    filter->SetUpperThreshold(3000);
    filter->SetInsideValue(1000);
    filter->SetOutsideValue(0);

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(JoinPath(outputDir, "itk_threshold.dcm"));
    writer->SetInput(filter->GetOutput());
    writer->SetImageIO(gdcmIO);

    try {
        writer->Update();
        std::cout << "Saved to '" << writer->GetFileName() << "'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

void ITKTests::TestResampling(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [ITK] Resampling ---" << std::endl;
    
    using PixelType = signed short;
    const unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;
    using ReaderType = itk::ImageFileReader<ImageType>;
    using WriterType = itk::ImageFileWriter<ImageType>;
    
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    
    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    reader->SetImageIO(gdcmIO);
    
    try {
        reader->Update();
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Exception: " << err << std::endl;
        return;
    }
    
    ImageType::Pointer inputImage = reader->GetOutput();
    ImageType::SpacingType inputSpacing = inputImage->GetSpacing();
    ImageType::SizeType inputSize = inputImage->GetLargestPossibleRegion().GetSize();
    
    std::cout << "Original Spacing: " << inputSpacing << std::endl;
    std::cout << "Original Size: " << inputSize << std::endl;
    
    ImageType::SpacingType outputSpacing;
    outputSpacing.Fill(1.0);
    
    ImageType::SizeType outputSize;
    outputSize[0] = static_cast<unsigned long>(inputSize[0] * inputSpacing[0] / outputSpacing[0]);
    outputSize[1] = static_cast<unsigned long>(inputSize[1] * inputSpacing[1] / outputSpacing[1]);
    outputSize[2] = static_cast<unsigned long>(inputSize[2] * inputSpacing[2] / outputSpacing[2]);

    using TransformType = itk::IdentityTransform<double, Dimension>;
    using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
    using ResampleFilterType = itk::ResampleImageFilter<ImageType, ImageType>;
    
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetInput(inputImage);
    resampler->SetSize(outputSize);
    resampler->SetOutputSpacing(outputSpacing);
    resampler->SetOutputOrigin(inputImage->GetOrigin());
    resampler->SetOutputDirection(inputImage->GetDirection());
    resampler->SetTransform(TransformType::New());
    resampler->SetInterpolator(InterpolatorType::New());
    resampler->SetDefaultPixelValue(0);
    
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(JoinPath(outputDir, "itk_resampled.dcm"));
    writer->SetInput(resampler->GetOutput());
    writer->SetImageIO(gdcmIO);
    
    try {
        writer->Update();
        std::cout << "Saved to '" << writer->GetFileName() << "'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

void ITKTests::TestAdaptiveHistogram(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [ITK] Adaptive Histogram Equalization ---" << std::endl;
    using PixelType = signed short;
    const unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;
    using ReaderType = itk::ImageFileReader<ImageType>;
    using WriterType = itk::ImageFileWriter<ImageType>;
    using EqualizeType = itk::AdaptiveHistogramEqualizationImageFilter<ImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);

    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    reader->SetImageIO(gdcmIO);

    try {
        reader->Update();
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Exception: " << err << std::endl;
        return;
    }

    EqualizeType::Pointer equalizer = EqualizeType::New();
    equalizer->SetInput(reader->GetOutput());
    equalizer->SetAlpha(0.3);
    equalizer->SetBeta(0.3);

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(JoinPath(outputDir, "itk_histogram_eq.dcm"));
    writer->SetInput(equalizer->GetOutput());
    writer->SetImageIO(gdcmIO);

    try {
        writer->Update();
        std::cout << "Saved to '" << writer->GetFileName() << "'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

void ITKTests::TestSliceExtraction(const std::string& filename, const std::string& outputDir) {
    std::cout << "--- [ITK] Slice Extraction ---" << std::endl;
    using PixelType = signed short;
    using InputImageType = itk::Image<PixelType, 3>;
    using SliceImageType = itk::Image<unsigned char, 2>;
    using ReaderType = itk::ImageFileReader<InputImageType>;
    using ExtractType = itk::ExtractImageFilter<InputImageType, SliceImageType>;
    using RescaleType = itk::RescaleIntensityImageFilter<SliceImageType, SliceImageType>;
    using WriterType = itk::ImageFileWriter<SliceImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);

    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    reader->SetImageIO(gdcmIO);

    try {
        reader->Update();
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Exception: " << err << std::endl;
        return;
    }

    InputImageType::RegionType region = reader->GetOutput()->GetLargestPossibleRegion();
    InputImageType::SizeType size = region.GetSize();
    InputImageType::IndexType start = region.GetIndex();
    start[2] = region.GetIndex()[2] + (size[2] / 2);
    size[2] = 0;

    ExtractType::Pointer extract = ExtractType::New();
    extract->SetInput(reader->GetOutput());
    extract->SetExtractionRegion({start, size});
    extract->SetDirectionCollapseToSubmatrix();

    RescaleType::Pointer rescale = RescaleType::New();
    rescale->SetInput(extract->GetOutput());
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(255);

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(JoinPath(outputDir, "itk_slice.png"));
    writer->SetInput(rescale->GetOutput());

    try {
        writer->Update();
        std::cout << "Saved middle slice PNG to '" << writer->GetFileName() << "'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

#else
void ITKTests::RegisterCommands(CommandRegistry&) {}
void ITKTests::TestCannyEdgeDetection(const std::string&, const std::string&) { std::cout << "ITK not enabled." << std::endl; }
void ITKTests::TestGaussianSmoothing(const std::string&, const std::string&) {}
void ITKTests::TestBinaryThresholding(const std::string&, const std::string&) {}
void ITKTests::TestResampling(const std::string&, const std::string&) {}
void ITKTests::TestAdaptiveHistogram(const std::string&, const std::string&) {}
void ITKTests::TestSliceExtraction(const std::string&, const std::string&) {}
#endif
