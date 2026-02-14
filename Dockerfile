FROM ubuntu:22.04

# Install build essentials, cmake, git, and python
RUN apt-get update && \
    apt-get install -y build-essential cmake git python3 python3-pip wget && \
    rm -rf /var/lib/apt/lists/*

# Install Manus API Hub client
RUN pip3 install manus-api-hub

# Set up the working directory
WORKDIR /app

# Copy the project files into the container
COPY . .

# Create a build directory
RUN mkdir build

# Configure the project with CMake
RUN cd build && cmake ..

# Build the project
RUN cd build && make
