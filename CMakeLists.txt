cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

project("AI-Aimbot")

include(ExternalProject)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/ScreenCaptureService/thirdparty/cmakemodules/)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(AIMBOT_INSTALL_PATH ${PROJECT_SOURCE_DIR}/aimbot_release)

ExternalProject_Add(
  openpose_proj
  PREFIX openpose
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/openpose"
  BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/openpose"
  INSTALL_COMMAND ""
  CMAKE_CACHE_ARGS -DBUILD_EXAMPLES:BOOL=OFF)
ExternalProject_Get_Property(openpose_proj BINARY_DIR)
set(OPENPOSE_BINARY_DIR ${BINARY_DIR})

add_library(openpose INTERFACE)
target_compile_definitions(openpose INTERFACE NOMINMAX)
target_include_directories(openpose
                           INTERFACE ${CMAKE_SOURCE_DIR}/openpose/include)

if(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
  target_link_libraries(
    openpose
    INTERFACE
      ${OPENPOSE_BINARY_DIR}/src/openpose/${CMAKE_BUILD_TYPE}/openpose.lib)
else()
  target_link_libraries(
    openpose
    INTERFACE
      ${OPENPOSE_BINARY_DIR}/src/openpose/${CMAKE_BUILD_TYPE}/openposed.lib)
endif()

set(targets ScreenCaptureService aimbot)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)

# download openpose model
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/openpose/models/getModels.bat
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/openpose/models/)
