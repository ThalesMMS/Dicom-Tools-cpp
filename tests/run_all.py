#
# run_all.py
# DicomToolsCpp
#
# Runs the compiled CLI against each module’s test command and checks for expected output artifacts.
#
# Thales Matheus Mendonça Santos - November 2025

import subprocess
import os
import sys

# Configuration
BUILD_DIR = "build"
EXECUTABLE = os.path.join(BUILD_DIR, "DicomTools")
INPUT_FILE = "input/dcm_series/IM-0001-0190.dcm"

# Ensure we fail early if the binary has not been built
if not os.path.exists(EXECUTABLE):
    print(f"Error: Executable not found at {EXECUTABLE}")
    sys.exit(1)

def run_test(command, description):
    print(f"Testing: {description}...")
    cmd = [EXECUTABLE, command]
    # If specific file needed, append it. The tool auto-detects, but consistent to be explicit if we knew the file.
    # cmd.append(INPUT_FILE) 
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"  [FAILED] Return code: {result.returncode}")
        print(result.stderr)
        return False
    else:
        print(f"  [PASS]")
        return True

# Check outputs
def check_file(filename):
    # Simple existence check; contents are validated manually when needed
    filepath = os.path.join("output", filename)
    if os.path.exists(filepath):
        print(f"  [OK] Output file generated: {filepath}")
        return True
    else:
        print(f"  [FAIL] Missing output file: {filepath}")
        return False

# Main
tests_passed = True

print("========================================")
print("      Automated Test Suite              ")
print("========================================")

# GDCM
if run_test("test-gdcm", "GDCM Features"):
    check_file("gdcm_anon.dcm")
    check_file("gdcm_raw.dcm")
    check_file("gdcm_reuid.dcm")
    check_file("gdcm_dump.txt")
    check_file("gdcm_jpeg2000.dcm")
    check_file("gdcm_rle.dcm")
    check_file("gdcm_jpegls.dcm")
    check_file("gdcm_stats.txt")
    check_file("gdcm_series_index.csv")
    check_file("gdcm_preview.pgm")
else:
    tests_passed = False

# DCMTK
if run_test("test-dcmtk", "DCMTK Features"):
    check_file("dcmtk_modified.dcm")
    check_file("dcmtk_pixel_output.ppm")
    check_file("dcmtk_jpeg_lossless.dcm")
    check_file("dcmtk_jpeg_baseline.dcm")
    check_file("dcmtk_rle.dcm")
    check_file("dcmtk_raw_dump.bin")
    check_file("dcmtk_explicit_vr.dcm")
    check_file("dcmtk_metadata.txt")
    check_file("dcmtk_preview.bmp")
    check_file("dicomdir_media/DICOMDIR")
else:
    tests_passed = False

# ITK
if run_test("test-itk", "ITK Features"):
    check_file("itk_canny.dcm")
    check_file("itk_gaussian.dcm")
    check_file("itk_median.dcm")
    check_file("itk_threshold.dcm")
    check_file("itk_otsu.dcm")
    check_file("itk_resampled.dcm")
    check_file("itk_aniso.dcm")
    check_file("itk_histogram_eq.dcm")
    check_file("itk_slice.png")
    check_file("itk_mip.png")
    check_file("itk_volume.nrrd")
    check_file("itk_volume.nii.gz")
else:
    tests_passed = False

# VTK
if run_test("test-vtk", "VTK Features"):
    check_file("vtk_export.vti")
    check_file("vtk_resampled.vti")
    check_file("vtk_volume.nii.gz")
    check_file("vtk_isosurface.stl")
    check_file("vtk_mpr_slice.png")
    check_file("vtk_mip.png")
    check_file("vtk_threshold_mask.vti")
    check_file("vtk_metadata.txt")
    check_file("vtk_stats.txt")
else:
    tests_passed = False

print("========================================")
if tests_passed:
    print("ALL TESTS PASSED")
    sys.exit(0)
else:
    print("SOME TESTS FAILED")
    sys.exit(1)
