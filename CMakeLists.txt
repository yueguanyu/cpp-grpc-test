cmake_minimum_required(VERSION 3.0.0)
project(cpp-grpc-test VERSION 0.1.0)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
    add_definitions(-D_WIN32_WINNT=0x600)
endif()

if(GRPC_AS_SUBMODULE)
    # One way to build a projects that uses gRPC is to just include the
    # entire gRPC project tree via "add_subdirectory".
    # This approach is very simple to use, but the are some potential
    # disadvantages:
    # * it includes gRPC's CMakeLists.txt directly into your build script
    #   without and that can make gRPC's internal setting interfere with your
    #   own build.
    # * depending on what's installed on your system, the contents of submodules
    #   in gRPC's third_party/* might need to be available (and there might be
    #   additional prerequisites required to build them). Consider using
    #   the gRPC_*_PROVIDER options to fine-tune the expected behavior.
    #
    # A more robust approach to add dependency on gRPC is using
    # cmake's ExternalProject_Add (see cmake_externalproject/CMakeLists.txt).
    
    # Include the gRPC's cmake build (normally grpc source code would live
    # in a git submodule called "third_party/grpc", but this example lives in
    # the same repository as gRPC sources, so we just look a few directories up)
    add_subdirectory(/usr/local/Cellar/grpc ${CMAKE_CURRENT_BINARY_DIR}/grpc EXCLUDE_FROM_ALL)
    message(STATUS "Using gRPC via add_subdirectory.")
    
    # After using add_subdirectory, we can now use the grpc targets directly from
    # this build.
    set(_PROTOBUF_LIBPROTOBUF libprotobuf)
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
    set(_GRPC_GRPCPP_UNSECURE grpc++_unsecure)
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
else()
    # This branch assumes that gRPC and all its dependencies are already installed
    # on this system, so they can be located by find_package().

    # Find Protobuf installation
    # Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
    set(protobuf_MODULE_COMPATIBLE TRUE)
    message(STATUS "CMAKE_ROOT ${CMAKE_ROOT}")
    find_package(Protobuf CONFIG REQUIRED)
    message(STATUS "Using protobuf ${protobuf_VERSION}")

    set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

    # Find gRPC installation
    # Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
    find_package(gRPC CONFIG REQUIRED)
    message(STATUS "Using gRPC ${gRPC_VERSION}")

    set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
    # set(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
endif()

# Proto file
get_filename_component(hw_proto "./proto/helloworld/helloworld.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h")
add_custom_command(
    OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
    DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Targets greeter_[async_](client|server)
foreach(_target
    greeter_client greeter_server
    greeter_async_client greeter_async_server)
    add_executable(${_target} "${_target}.cc"
        ${hw_proto_srcs}
        ${hw_grpc_srcs})
    target_link_libraries(${_target}
        ${_GRPC_GRPCPP_UNSECURE}
        ${_PROTOBUF_LIBPROTOBUF})
endforeach()

# include(CTest)
# enable_testing()

# add_executable(cpp-grpc-test main.cpp)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
