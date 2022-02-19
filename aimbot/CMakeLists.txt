add_executable(aimbot main.cpp)

target_link_libraries(aimbot screencaptureapi opencv openpose spdlog)

install(TARGETS aimbot RUNTIME DESTINATION ${AIMBOT_INSTALL_PATH})

# copy screen capture server and client dll related
install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/ScreenCaptureService/bin/
  DESTINATION ${AIMBOT_INSTALL_PATH}/
  FILES_MATCHING
  PATTERN "*.dll"
  PATTERN "*.exe"
  PATTERN "*.json")

# copy openpose depencies
install(
  DIRECTORY ${CMAKE_BINARY_DIR}/openpose/bin/
  DESTINATION ${AIMBOT_INSTALL_PATH}/
  FILES_MATCHING
  PATTERN "*.dll")

# copy openpose dll
install(
  DIRECTORY ${CMAKE_BINARY_DIR}/openpose/x64/${CMAKE_BUILD_TYPE}/
  DESTINATION ${AIMBOT_INSTALL_PATH}/
  FILES_MATCHING
  PATTERN "*.dll"
  PATTERN "ALL_BUILD" EXCLUDE)

# copy openpose model
install(DIRECTORY ${PROJECT_SOURCE_DIR}/openpose/models
        DESTINATION ${AIMBOT_INSTALL_PATH})
