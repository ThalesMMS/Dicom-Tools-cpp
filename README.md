# Dicom-Tools-cpp

A modular C++ repository designed to demonstrate, test, and benchmark functionalities of major medical imaging libraries:
- **GDCM** (Grassroots DICOM)
- **DCMTK** (DICOM Toolkit)
- **ITK** (Insight Segmentation and Registration Toolkit)
- **VTK** (Visualization Toolkit)

## Features Implemented

| Library | Feature | Description |
| :--- | :--- | :--- |
| **GDCM** | **Anonymization** | Redacts Patient Name, ID, and Birth Date. |
| | **Decompression** | Transcodes compressed DICOMs to Raw (Implicit VR Little Endian). |
| | **Tag Inspection** | Efficiently reads and displays specific tags. |
| **DCMTK** | **Tag Modification** | Modifies metadata (e.g., PatientID) and saves new files. |
| | **Pixel Extraction** | Extracts pixel data and exports as PGM/PPM images. |
| **ITK** | **Edge Detection** | Applies Canny Edge Detection filter. |
| | **Smoothing** | Reduces noise using Discrete Gaussian Smoothing. |
| | **Segmentation** | Segments structures using Binary Thresholding. |
| | **Resampling** | Resamples volumes to isotropic spacing (1x1x1mm). |
| **VTK** | **3D Mesh Generation** | Generates STL surfaces using Marching Cubes. |
| | **MPR** | Extracts 2D slices (Multi-Planar Reformatting) from 3D volumes. |
| | **Volume Export** | Converts DICOM series to VTK XML Image Data (`.vti`). |

## Prerequisites

- **CMake** (3.15 or higher)
- **C++ Compiler** (C++17 support required)
- **Python 3** (for automated testing)

### Dependency Management

You can install the required libraries via your package manager (e.g., `brew install gdcm dcmtk itk vtk`). 

Alternatively, this repo includes a script to download and build all dependencies locally:

```bash
./scripts/build_deps.sh
```
*This will install dependencies into `deps/install`, which the CMake configuration automatically detects.*

## Building

1.  Create a build directory:
    ```bash
    mkdir build
    cd build
    ```

2.  Configure with CMake:
    ```bash
    cmake ..
    ```

3.  Build the project:
    ```bash
    cmake --build .
    ```

## Usage

The project generates a single executable `DicomTools` in the `build` directory. It supports subcommands to target specific libraries.

**Syntax:**
```bash
./build/DicomTools <command> [optional: /path/to/file.dcm]
```
*If no file is provided, it auto-detects the first `.dcm` file in the `input/` folder.*

**Commands:**
- `test-gdcm`: Run GDCM specific tests.
- `test-dcmtk`: Run DCMTK specific tests.
- `test-itk`: Run ITK specific tests.
- `test-vtk`: Run VTK specific tests.
- `all`: Run all available tests.

**Example:**
```bash
./build/DicomTools test-itk input/dcm_series/IM-0001-0190.dcm
```

## Automated Testing

A Python script is provided to run the full suite of tests and verify that the expected output files are generated correctly in the `output/` directory.

```bash
python3 tests/run_all.py
```

## Project Structure

- `src/modules/`: Modular implementation for each library (GDCM, DCMTK, ITK, VTK).
- `input/`: Directory for test DICOM images.
- `output/`: All generated files (images, meshes, anonymized DICOMs) are saved here.
- `scripts/`: Helper scripts for dependency setup.