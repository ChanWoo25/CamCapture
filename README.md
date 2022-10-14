### CwCapture Library

when you utilize v4l2 library supported camera device, the absence of knowledge about v4l2 library can be bottleneck.

This library ease a way to control device.

this library supports only UYVY image format only. But easiily extended to other format with v4l2 documentation.

### When using shared custom lib
```bash
cmake_minimum_required(VERSION 3.10)
project(CwCapture)

find_package(OpenCV 3.2 REQUIRED)
message(STATUS "OpenCV Version: " ${OpenCV_VERSION})

add_library(CwCapture SHARED IMPORTED)
set_target_properties(CwCapture PROPERTIES
IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libCwCapture.so)

add_executable(test_${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)
target_include_directories(test_${PROJECT_NAME} 
PUBLIC 
  ${OpenCV_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(test_${PROJECT_NAME} PRIVATE ${PROJECT_NAME} ${OpenCV_LIBRARIES} CwCapture)
target_compile_features(test_${PROJECT_NAME} PRIVATE cxx_std_14)
set_target_properties(test_${PROJECT_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin")
```