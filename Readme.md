# nc-inspect

Utility for inspecting the contents of NetCDF files. 

NOTE: Currently a prototype in need of improvement, see [issues](https://github.com/CurlyNikolai/nc-inspect/issues). 

## Requirements

This project is tested and packaged for **Ubuntu 22.04 (jammy)** and **Ubuntu 24.04 (noble)**. 

The easiest way to take the software into use is with the provided `.deb` package, see [releases](https://github.com/CurlyNikolai/nc-inspect/releases). To build and run the software for development, it is recommended to use the included Docker environment, which handles all required dependencies. 

## Dependencies

This project uses dependencies that are installed via the provided Docker environment, listed here for reference:

- [ImGui](https://github.com/ocornut/imgui) - Immediate Mode GUI library
- [ImPlot](https://github.com/epezent/implot) - Immediate Mode Plotting library
- [NetCDF](https://docs.unidata.ucar.edu/netcdf-c) - Library for NetCDF file access
- [GLFW](https://www.glfw.org/) - Library for window and input handling
- [OpenGL](https://www.opengl.org/) - Graphics library
- [fmt](https://github.com/fmtlib/fmt) - Formatting library

See `docker/Dockerfile` for a complete list and how they are installed with `apt-get`. 

## Usage

### Installing the `.deb` package

To install, run the `.deb` package on a supported Ubuntu release:

```bash
apt update
apt install ./nc-inspect-0.1.0-jammy.deb
```

### Running the software

To test that the install was succesful, simply run the program for a NetCDF file:

```bash
nc-inspect data/test.nc
```

The `nc-inspect` window should appear:
<br>
<center><img src="images/nc-inspect.png" alt="nc-inspect image" width="750"></center>
<br>

You can also provide several files at the same time:

```bash
nc-inspect file1.nc file2.nc
nc-inspect file*.nc
```

Note that the software will not start if no input NetCDF files are provided. 

## Development

### Docker

For development it is recommended to use the provided Docker environment. To build and enter the container shell run:

```bash
docker build --target dev -t nc-inspect-dev -f docker/Dockerfile .
./docker/run.sh
```

### Building

Note that the `docker/run.sh` script runs an image named `$DEV_IMAGE_NAME:latest` where the first part is obtained from the `docker/config.sh` file.

To build the source code, and start the software, run for example:

```bash
cmake -B build -S src
cmake --build build
./build/nc-inspect data/test.nc
```