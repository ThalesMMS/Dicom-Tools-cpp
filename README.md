# Dicom-Tools-cpp

This C++ project is designed to test functionalities of various DICOM and medical imaging libraries:
- **GDCM** (Grassroots DICOM)
- **DCMTK** (DICOM Toolkit)
- **VTK** (Visualization Toolkit)
- **ITK** (Insight Segmentation and Registration Toolkit)

## Project Structure

- `src/`: Contains the source code (`main.cpp`).
- `input/`: Place your test `.dcm` images here. The program will scan this directory.
- `CMakeLists.txt`: CMake configuration file to find libraries and build the project.

## Prerequisites

You need to have the libraries you want to test installed on your system.
- CMake (3.15 or higher)
- C++ Compiler (supporting C++17)
- Libraries: ITK, VTK, DCMTK, GDCM (optional, but recommended for full testing)

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
    *Note: If libraries are installed in non-standard locations, you may need to specify their paths (e.g., `-DITK_DIR=/path/to/itk`).*

3.  Build:
    ```bash
    cmake --build .
    ```

## Usage

Run the executable from the build directory:

```bash
./DicomTools
```

The program will:
1.  List files found in the `input/` directory.
2.  Report which libraries were successfully found and linked.
3.  Run a basic "smoke test" (instantiation of a core object) for each enabled library.
