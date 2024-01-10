# CamCapture

## Purpose

when you utilize v4l2 library supported camera device, the absence of knowledge about v4l2 library can be bottleneck.

This library ease a way to control device.

this library supports only UYVY image format only. But easiily extended to other format with [v4l2 documentation](https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/v4l2.html).

## Build

```bash
mkdir build && cd build
cmake ..
make
```

## Link to your cmake project

You'll get `libcapture.so` library file in `lib` directory if you followed **Build** section.

Then, copy library file (`lib/libcapture.so`) into `/home/gildong/project/lib`, if your cmake project's ROOT path is `/home/gildong/project`. And put following code segment into your `CMakeLists.txt`.

Note that the same OpenCV version which is used previously to build the library must be used.

```bash
...

# There is no specific opencv version we need for this library
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV Version: " ${OpenCV_VERSION})

add_library(capture SHARED IMPORTED)
set_target_properties(capture PROPERTIES
IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcapture.so)

add_executable(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)
target_include_directories(${PROJECT_NAME}
PUBLIC
  ${OpenCV_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${OpenCV_LIBRARIES}
  capture)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)
set_target_properties(${PROJECT_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin")
```
