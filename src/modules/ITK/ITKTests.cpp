#include "ITKTestInterface.h"
#include <iostream>

#ifdef USE_ITK
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkGDCMImageIO.h"

#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"

void ITKTests::TestCannyEdgeDetection(const std::string& filename) {
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
    
    // Rescale/Cast to Unsigned Char for DICOM writing
    RescaleType::Pointer rescaler = RescaleType::New();
    rescaler->SetInput(filter->GetOutput());
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName("output/itk_canny.dcm");
    writer->SetInput(rescaler->GetOutput());
    writer->SetImageIO(gdcmIO);

    try {
        writer->Update();
        std::cout << "Saved to 'output/itk_canny.dcm'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

void ITKTests::TestGaussianSmoothing(const std::string& filename) {
    std::cout << "--- [ITK] Gaussian Smoothing ---" << std::endl;
    
    using PixelType = signed short;
    const unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;
    using ReaderType = itk::ImageFileReader<ImageType>;
    using WriterType = itk::ImageFileWriter<ImageType>;
    using FilterType = itk::DiscreteGaussianImageFilter<ImageType, ImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    
    // Using GDCM IO
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
    writer->SetFileName("output/itk_gaussian.dcm");
    writer->SetInput(filter->GetOutput());
    writer->SetImageIO(gdcmIO);

    try {
        writer->Update();
        std::cout << "Saved to 'output/itk_gaussian.dcm'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

void ITKTests::TestBinaryThresholding(const std::string& filename) {
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
    filter->SetLowerThreshold(200); // Arbitrary threshold for bone/contrast
    filter->SetUpperThreshold(3000);
    filter->SetInsideValue(1000);
    filter->SetOutsideValue(0);

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName("output/itk_threshold.dcm");
    writer->SetInput(filter->GetOutput());
    writer->SetImageIO(gdcmIO);

    try {
        writer->Update();
        std::cout << "Saved to 'output/itk_threshold.dcm'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

void ITKTests::TestResampling(const std::string& filename) {
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
    
    // Target spacing: 1x1x1 mm
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
    writer->SetFileName("output/itk_resampled.dcm");
    writer->SetInput(resampler->GetOutput());
    writer->SetImageIO(gdcmIO);
    
    try {
        writer->Update();
        std::cout << "Saved to 'output/itk_resampled.dcm'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

#else
void ITKTests::TestCannyEdgeDetection(const std::string&) { std::cout << "ITK not enabled." << std::endl; }
void ITKTests::TestGaussianSmoothing(const std::string&) {}
void ITKTests::TestBinaryThresholding(const std::string&) {}
void ITKTests::TestResampling(const std::string&) {}
#endif