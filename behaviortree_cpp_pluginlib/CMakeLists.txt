# Copyright (c) 2025-present Polymath Robotics, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.22)
project(behaviortree_cpp_pluginlib)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
  add_link_options(-Wl,-no-undefined)
endif()

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

# Create library
add_library(${PROJECT_NAME} SHARED
  src/factory.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    behaviortree_cpp::behaviortree_cpp
    pluginlib::pluginlib
  PRIVATE
    rcutils::rcutils
    ament_index_cpp::ament_index_cpp
)

# Install and export resources
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}_TARGETS
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(
  EXPORT ${PROJECT_NAME}_TARGETS
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION share/${PROJECT_NAME}/cmake
)
install(
  FILES src/autoregistrar.cpp.in src/plugin_description.xml.in
  DESTINATION share/${PROJECT_NAME}
)
install(
  DIRECTORY include/
  DESTINATION include
)
install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  ament_auto_find_test_dependencies()

  set(arg_TARGET test_register)
  configure_file(
    src/autoregistrar.cpp.in
    ${PROJECT_BINARY_DIR}/test/autoregistrar.cpp
    @ONLY
  )

  ament_add_gtest(test_register
    test/test_register.cpp
    ${PROJECT_BINARY_DIR}/test/autoregistrar.cpp
  )
  target_link_libraries(test_register ${PROJECT_NAME})
endif()

ament_export_targets(${PROJECT_NAME}_TARGETS HAS_LIBRARY_TARGET)
ament_export_dependencies(behaviortree_cpp pluginlib)
ament_package(
  CONFIG_EXTRAS "${PROJECT_NAME}-extras.cmake"
)
