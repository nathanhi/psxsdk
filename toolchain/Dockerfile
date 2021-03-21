FROM debian:buster-slim AS build
# Intermediate container for the GCC/binutils build

# Install build tools
RUN cp -rfv /etc/apt/sources.list /etc/apt/sources.list.d/deb-src.list && \
    sed -i 's/^deb /deb-src /g' /etc/apt/sources.list.d/deb-src.list
RUN apt-get update && \
    apt-get build-dep -y --no-install-recommends clang llvm && \
    apt-get install -y --no-install-recommends cmake patch ca-certificates

# Copy CMakeLists.txt and patches to /opt/psx/toolchain
RUN mkdir -p /opt/psx/toolchain/patches
COPY CMakeLists.txt /opt/psx/toolchain/
COPY patches /opt/psx/toolchain/patches
WORKDIR /opt/psx/toolchain

# Build toolchain & clean up
RUN cmake .; cmake --build .
RUN rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake CMakeLists.txt Makefile patches src tmp

# Final psxsdk toolchain container
FROM debian:buster-slim

# Install CMake & build system
RUN apt-get update; apt-get install -y --no-install-recommends ninja-build make

# Copy pre-built GCC from intermediate container
RUN mkdir /opt/psx
COPY --from=build /opt/psx /opt/psx

# Download & run CMake installer
ARG CMAKE_VERSION=3.19.7
COPY cmake /opt/psx/cmake
RUN /opt/psx/cmake/install_cmake.sh ${CMAKE_VERSION}

# Copy CMake platform
ENV PATH=/opt/psx/toolchain/bin:$PATH
ENV LD_LIBRARY_PATH=/opt/psx/toolchain/lib
RUN ln -s /opt/psx/cmake/Modules/Platform/psx.cmake /share/cmake-$(echo ${CMAKE_VERSION} | cut -f1-2 -d".")/Modules/Platform/psx.cmake

# Specify compiler
ENV CC=mipsel-unknown-elf-gcc
ENV CXX=mipsel-unknown-elf-gcc

# Always copy Dockerfile as reference
COPY Dockerfile /