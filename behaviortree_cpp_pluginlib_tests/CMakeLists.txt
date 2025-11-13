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
project(behaviortree_cpp_pluginlib_tests)

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

if(BUILD_TESTING)
  ament_auto_find_test_dependencies()

  # Create test libraries
  add_library(test_plugin_a SHARED test/plugin_a.cpp)
  target_link_libraries(test_plugin_a PUBLIC behaviortree_cpp_pluginlib::behaviortree_cpp_pluginlib)
  register_behaviortree_cpp_plugin(test_plugin_a)

  add_library(test_plugin_b SHARED test/plugin_b.cpp)
  target_link_libraries(test_plugin_b PRIVATE behaviortree_cpp_pluginlib::behaviortree_cpp_pluginlib)
  register_behaviortree_cpp_plugin(test_plugin_b)

  # In the special case of a test-only package that isn't normally installed on target systems,
  # we may install the test targets to run install-space testing on them (pluginlib loading)
  install(
    TARGETS test_plugin_a test_plugin_b
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
  )

  polymath_add_catch2_test(test_factory test/test_factory.cpp test_plugin_a)
  target_link_libraries(test_factory behaviortree_cpp_pluginlib::behaviortree_cpp_pluginlib)
endif()

ament_package()
