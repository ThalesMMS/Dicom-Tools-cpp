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
| | **Tag Inspection** | Reads and displays common tags. |
| | **UID Rewrite** | Regenerates Study/Series/SOP Instance UIDs. |
| | **Dataset Dump** | Writes a verbose text dump of the DICOM dataset. |
| | **JPEG2000 Transcode** | Tests JPEG2000 lossless codec support. |
| **DCMTK** | **Tag Modification** | Modifies metadata (e.g., PatientID) and saves new files. |
| | **Pixel Extraction** | Extracts pixel data and exports as PGM/PPM images. |
| | **JPEG Lossless** | Re-encodes to JPEG Lossless (Process 14 SV1). |
| | **Raw Dump** | Writes raw pixel buffer for regression checks. |
| **ITK** | **Edge Detection** | Applies Canny Edge Detection filter. |
| | **Smoothing** | Reduces noise using Discrete Gaussian Smoothing. |
| | **Segmentation** | Segments structures using Binary Thresholding. |
| | **Resampling** | Resamples volumes to isotropic spacing (1x1x1mm). |
| | **Histogram EQ** | Adaptive histogram equalization for contrast. |
| | **Slice Export** | Extracts the middle slice to PNG. |
| **VTK** | **3D Mesh Generation** | Generates STL surfaces using Marching Cubes. |
| | **MPR** | Extracts 2D slices (Multi-Planar Reformatting) from 3D volumes. |
| | **Volume Export** | Converts DICOM series to VTK XML Image Data (`.vti`). |
| | **Threshold Mask** | Builds a binary mask volume from a HU window. |
| | **Metadata Export** | Writes patient/study metadata to text. |

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
./build/DicomTools <command> [options]
```

**Useful options:**
- `-i, --input <path>`: DICOM file or series directory (defaults to first `.dcm` under `input/`).
- `-o, --output <dir>`: Output directory (defaults to `output/`).
- `-l, --list`: Show all registered commands.
- `-h, --help`: CLI help.

**High-level commands:**
- `test-gdcm`, `test-dcmtk`, `test-itk`, `test-vtk`: Run all feature tests in each module.
- `all`: Run every available test (shortcut to the above).

**Granular commands (examples):**
- `gdcm:anonymize`, `gdcm:dump`, `gdcm:transcode-j2k`
- `dcmtk:jpeg-lossless`, `dcmtk:raw-dump`
- `itk:histogram`, `itk:slice`
- `vtk:mask`, `vtk:metadata`, `vtk:isosurface`

**Examples:**
```bash
./build/DicomTools --list
./build/DicomTools test-itk --input input/dcm_series/IM-0001-0190.dcm --output tmp/itk_out
./build/DicomTools vtk:mask
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
