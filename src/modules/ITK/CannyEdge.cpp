#include "ITKTestInterface.h"
#include <iostream>

#ifdef USE_ITK
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkGDCMImageIO.h"

void ITKTests::TestCannyEdgeDetection(const std::string& filename) {
    std::cout << "--- [ITK] Canny Edge Detection ---" << std::endl;
    
    using PixelType = float;
    const unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;
    using ReaderType = itk::ImageFileReader<ImageType>;
    using WriterType = itk::ImageFileWriter<ImageType>;
    using FilterType = itk::CannyEdgeDetectionImageFilter<ImageType, ImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    
    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    reader->SetImageIO(gdcmIO);

    try {
        reader->Update();
        std::cout << "Image read successfully." << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Exception: " << err << std::endl;
        return;
    }

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(reader->GetOutput());
    filter->SetVariance(2.0);
    filter->SetUpperThreshold(0.05);
    filter->SetLowerThreshold(0.02);

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName("itk_canny.dcm");
    writer->SetInput(filter->GetOutput());
    writer->SetImageIO(gdcmIO);

    try {
        writer->Update();
        std::cout << "Saved to 'itk_canny.dcm'" << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ITK Write Exception: " << err << std::endl;
    }
}

void ITKTests::TestGaussianSmoothing(const std::string& filename) {
    std::cout << "--- [ITK] Gaussian Smoothing (Not Implemented) ---" << std::endl;
}

void ITKTests::TestResampling(const std::string& filename) {
    std::cout << "--- [ITK] Resampling (Not Implemented) ---" << std::endl;
}

#else
void ITKTests::TestCannyEdgeDetection(const std::string&) { std::cout << "ITK not enabled." << std::endl; }
void ITKTests::TestGaussianSmoothing(const std::string&) {}
void ITKTests::TestResampling(const std::string&) {}
#endif
