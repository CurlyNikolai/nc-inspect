cmake_minimum_required(VERSION 3.22.1)

if (NOT DEFINED PROJECT_VERSION)
    set(PROJECT_VERSION 0.0.0)
endif()

project(nc-inspect VERSION ${PROJECT_VERSION})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Paths
set(LIBS_DIR "/lib/third_party")
set(IMGUI_DIR "${LIBS_DIR}/imgui-docking")
set(IMGUI_BACKENDS_DIR "${IMGUI_DIR}/backends")
set(IMPLOT_DIR "${LIBS_DIR}/implot-master")

# Add imgui library
add_library(imgui
    ${IMGUI_DIR}/imgui.cpp 
    ${IMGUI_DIR}/imgui_demo.cpp 
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

# Add imgui backends library
add_library(imgui_backends
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)

target_link_libraries(imgui_backends imgui)
target_include_directories(imgui_backends PUBLIC ${IMGUI_DIR})

# Add implot library
add_library(implot
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
)
target_link_libraries(implot imgui)
target_include_directories(implot PUBLIC ${IMGUI_DIR})

# Find external packages
find_package(glfw3 REQUIRED)
find_package(fmt REQUIRED)
find_package(netCDF REQUIRED)
find_package(OpenGL REQUIRED OPTIONAL_COMPONENTS GLES3)

# Project source files
set(SOURCES 
    main.cpp
    Screen.cpp
    NetCDFReader.cpp
    ImguiWidgets.cpp
    VariableData.cpp
    PlotData.cpp
)

# Project header files
set(HEADERS 
    Screen.h
    NetCDFReader.h
    Node.h
    ImguiWidgets.h
    VariableData.h
    PlotData.h
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries and add include directories to executable
target_link_libraries(${PROJECT_NAME} PRIVATE GL imgui imgui_backends implot glfw fmt::fmt netCDF::netcdf)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR} ${IMPLOT_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Install debian copyright
install(FILES ../debian/copyright
    DESTINATION share/doc/${PROJECT_NAME}
)

# Basic package info
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NetCDF data visualizer")
set(CPACK_PACKAGE_VENDOR "${PROJECT_NAME} project")
set(CPACK_PACKAGE_LICENSE "MIT")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/CurlyNikolai/nc-inspect")

# Set package formats
set(CPACK_GENERATOR "DEB;TGZ")

# CPack configuration for .deb package
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PROJECT_NAME} developers 48209434+CurlyNikolai@users.noreply.github.com")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "This software is an utility to inspect NetCDF file contents and data visualization.")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)