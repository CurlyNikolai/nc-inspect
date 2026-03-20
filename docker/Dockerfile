ARG UBUNTU=jammy
# Image for developers
FROM ubuntu:${UBUNTU} AS dev

# Install debendancies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    # Build and development environment
    build-essential \
    sudo \
    cmake \
    unzip \
    xorg-dev \
    ninja-build \
    file \
    dpkg-dev \
    python3 \
    python3-numpy \
    python3-netcdf4 \
    # SW dependencies
    libgles2-mesa-dev \
    libnetcdf-dev \
    libglfw3-dev \
    libfmt-dev && \
    rm -rf /var/lib/apt/lists/*

# Create external lib folder
ARG LIB_EXT=/lib/third_party
RUN mkdir ${LIB_EXT}

# Define external library versions here
ARG IMGUI_VERSION=1.92.2b
ARG IMPLOT_VERSION="0.17"

# Download imgui (https://github.com/ocornut/imgui)
ADD https://github.com/ocornut/imgui/archive/refs/tags/v${IMGUI_VERSION}-docking.zip ${LIB_EXT}/master.zip
RUN unzip ${LIB_EXT}/master.zip -d ${LIB_EXT}/ && \
    rm -rf ${LIB_EXT}/master. && \
    mv ${LIB_EXT}/imgui-${IMGUI_VERSION}-docking ${LIB_EXT}/imgui-docking

# Download implot
ADD https://github.com/epezent/implot/archive/refs/tags/v${IMPLOT_VERSION}.zip ${LIB_EXT}/implot-${IMPLOT_VERSION}.zip
RUN unzip ${LIB_EXT}/implot-${IMPLOT_VERSION}.zip -d ${LIB_EXT}/ && \
    mv ${LIB_EXT}/implot-${IMPLOT_VERSION} ${LIB_EXT}/implot-master && \
    rm -rf ${LIB_EXT}/implot-${IMPLOT_VERSION}.zip

# Image for building
FROM dev AS build

ARG UBUNTU=jammy
ARG VERSION=0.1.0

WORKDIR /src
COPY . .
RUN cmake -GNinja -DCMAKE_BUILD_TYPE=RELEASE -DPROJECT_VERSION=${VERSION} -B build -S src && \
    cmake --build build && \
    cd build && cpack && \
    mv nc-inspect-${VERSION}-Linux.deb nc-inspect-${VERSION}-${UBUNTU}.deb

# Export debian package
FROM scratch AS export
ARG VERSION=0.1.0
COPY --from=build /src/build/*.deb /

# Image for users
ARG UBUNTU=jammy
FROM ubuntu:${UBUNTU} AS user
ARG VERSION=0.1.0
COPY --from=build /src/build/nc-inspect-*.deb /
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    ./nc-inspect-*.deb \
    libgl1 \
    libglfw3 \
    libx11-6 \
    libxrandr2 \
    libxinerama1 \
    libxcursor1 \
    libxi6 \
    libxss1 && \
    rm -rf /var/lib/apt/lists/*