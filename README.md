# cpp-grpc-test

cpp-grpc-test

# Build with Makefile

please install protobuf and grpc with grpc_cpp_plugin for generate code with the Makefile

# Build with CMake

If you want to use CMake to generate your Makefile for your project. please use cmake to install the protobuf package and the grpc package. You can follow the steps:

1. clone the gRPC package from GitHub `git clone https://github.com/grpc/grpc.git && cd grpc`
2. update all the submodule for gRPC package `git submodule update --init`
3. update protobuf to last version `cd third_party/protobuf && git pull origin master && git submodule update --init --recursive`
3. Build Protobuf using CMake `mkdir -p cmake/build && cd cmake/build && cmake .. && make && make install`
4. Build gRPC als submodule
    - cd to gRPC directory `cd ../../..`
    - `mkdir build_grpc && cd build_grpc`
    - `cmake ..`
    - `make && make install`
5. Congratulations! you can use CMake to build your project now!
