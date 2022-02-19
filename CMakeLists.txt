cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

project("AI-Aimbot")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_SOURCE_DIR}/ScreenCaptureService/thirdparty/cmakemodules/)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(targets ScreenCaptureService)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)
