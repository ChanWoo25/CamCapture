cmake_minimum_required(VERSION 3.10)
project(CwCapture)

find_package(OpenCV 3.2 REQUIRED)
message(STATUS "OpenCV Version: " ${OpenCV_VERSION})

add_library(${PROJECT_NAME} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CwCapture.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/v4l2utils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/v4l2utils.c
)
target_include_directories(${PROJECT_NAME} 
PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES
LIBRARY_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/lib")

add_executable(test_${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)
target_include_directories(test_${PROJECT_NAME} 
PUBLIC 
  ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(test_${PROJECT_NAME} PRIVATE ${PROJECT_NAME} ${OpenCV_LIBRARIES} CwCapture)
target_compile_features(test_${PROJECT_NAME} PRIVATE cxx_std_14)
set_target_properties(test_${PROJECT_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin")