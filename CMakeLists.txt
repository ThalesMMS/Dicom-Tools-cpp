cmake_minimum_required(VERSION 3.15)
project(DicomToolsCpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add local deps install dir to prefix path
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/deps/install")

# Pass the input directory to the source code
add_compile_definitions(INPUT_DIR="${CMAKE_SOURCE_DIR}/input")

# --- Find Packages ---

# ITK
find_package(ITK QUIET)
if(ITK_FOUND)
    include(${ITK_USE_FILE})
    message(STATUS "Found ITK: ${ITK_VERSION}")
    add_compile_definitions(USE_ITK)
else()
    message(WARNING "ITK not found. ITK tests will be disabled.")
endif()

# VTK
find_package(VTK QUIET)
if(VTK_FOUND)
    message(STATUS "Found VTK: ${VTK_VERSION}")
    add_compile_definitions(USE_VTK)
else()
    message(WARNING "VTK not found. VTK tests will be disabled.")
endif()

# DCMTK
find_package(DCMTK QUIET)
if(DCMTK_FOUND)
    include_directories(${DCMTK_INCLUDE_DIRS})
    message(STATUS "Found DCMTK: ${DCMTK_VERSION}")
    add_compile_definitions(USE_DCMTK)
else()
    message(WARNING "DCMTK not found. DCMTK tests will be disabled.")
endif()

# GDCM
find_package(GDCM QUIET)
if(GDCM_FOUND)
    include(${GDCM_USE_FILE})
    message(STATUS "Found GDCM: ${GDCM_VERSION}")
    add_compile_definitions(USE_GDCM)
else()
    message(WARNING "GDCM not found. GDCM tests will be disabled.")
endif()

# --- Executable ---
add_executable(DicomTools 
    src/main.cpp
    src/modules/GDCM/TagInspection.cpp
    src/modules/DCMTK/TagModification.cpp
    src/modules/ITK/CannyEdge.cpp
    src/modules/VTK/ImageExport.cpp
)

# --- Linking ---
if(ITK_FOUND)
    target_link_libraries(DicomTools PRIVATE ${ITK_LIBRARIES})
endif()

if(VTK_FOUND)
    target_link_libraries(DicomTools PRIVATE ${VTK_LIBRARIES})
endif()

if(DCMTK_FOUND)
    target_link_libraries(DicomTools PRIVATE ${DCMTK_LIBRARIES})
endif()

if(GDCM_FOUND)
    target_link_libraries(DicomTools PRIVATE ${GDCM_LIBRARIES})
endif()
