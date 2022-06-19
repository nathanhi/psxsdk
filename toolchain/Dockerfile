# Using CentOs 7 for broad compatibility (relatively old glibc)
FROM centos:7

# Install build tools
RUN yum groupinstall -y 'Development Tools'
RUN yum install -y epel-release && yum install -y cmake3 texinfo zlib-devel
RUN yum install -y dpkg

# Copy CMakeLists.txt and patches to /opt/psx/toolchain
#RUN mkdir -p /opt/psx/toolchain/patches
#COPY CMakeLists.txt /opt/psx/toolchain/
#COPY patches /opt/psx/toolchain/patches
#WORKDIR /opt/psx/toolchain

# Build toolchain & clean up
#RUN cmake .; cmake --build .
#RUN rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake CMakeLists.txt Makefile patches src tmp
