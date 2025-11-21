import subprocess
import os
import sys

# Configuration
BUILD_DIR = "build"
EXECUTABLE = os.path.join(BUILD_DIR, "DicomTools")
INPUT_FILE = "input/dcm_series/IM-0001-0190.dcm"

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
else:
    tests_passed = False

# DCMTK
if run_test("test-dcmtk", "DCMTK Features"):
    check_file("dcmtk_modified.dcm")
    check_file("dcmtk_pixel_output.ppm")
    check_file("dcmtk_jpeg_lossless.dcm")
    check_file("dcmtk_raw_dump.bin")
else:
    tests_passed = False

# ITK
if run_test("test-itk", "ITK Features"):
    check_file("itk_canny.dcm")
    check_file("itk_gaussian.dcm")
    check_file("itk_threshold.dcm")
    check_file("itk_resampled.dcm")
    check_file("itk_histogram_eq.dcm")
    check_file("itk_slice.png")
else:
    tests_passed = False

# VTK
if run_test("test-vtk", "VTK Features"):
    check_file("vtk_export.vti")
    check_file("vtk_isosurface.stl")
    check_file("vtk_mpr_slice.png")
    check_file("vtk_threshold_mask.vti")
    check_file("vtk_metadata.txt")
else:
    tests_passed = False

print("========================================")
if tests_passed:
    print("ALL TESTS PASSED")
    sys.exit(0)
else:
    print("SOME TESTS FAILED")
    sys.exit(1)
