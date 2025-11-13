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

#
# Register a library target as providing BehaviorTree.CPP plugins
#
# Example usage:
# register_behaviortree_cpp_plugin(my_library)
#
# :param TARGET: name of a valid CMake shared library target to export plugins from
# :type TARGET: string
#
# @public
#
function(register_behaviortree_cpp_plugin arg_TARGET)
  if(NOT arg_TARGET)
    message(FATAL_ERROR "register_behaviortree_cpp_plugin() called without TARGET argument")
  endif()

  get_filename_component(template_src "${behaviortree_cpp_pluginlib_DIR}" DIRECTORY)
  set(template_dest ${PROJECT_BINARY_DIR}/behaviortree_cpp_pluginlib/${arg_TARGET})

  # Define the AutoRegistrar plugin implementation class that registers the declared BT extensions
  configure_file(
    ${template_src}/autoregistrar.cpp.in
    ${template_dest}__autoregistrar.cpp
    @ONLY
  )
  target_sources("${arg_TARGET}" PRIVATE
    ${template_dest}__autoregistrar.cpp
  )

  # Create the pluginlib XML file to expose the above AutoRegistrar to pluginlib's ClassLoader
  configure_file(
    ${template_src}/plugin_description.xml.in
    ${template_dest}__btcpp_plugin_description.xml
    @ONLY
  )
  install(FILES ${template_dest}__btcpp_plugin_description.xml DESTINATION share/${PROJECT_NAME})

  # Mimics `pluginlib_export_plugin_description_file`, which assumes a file in CMAKE_CURRENT_SOURCE_DIR
  # Can't override by setting CMAKE_CURRENT_SOURCE_DIR because `install()` in that context needs a relative filename
  set(__PLUGINLIB_CATEGORY_CONTENT__behaviortree_cpp
    "${__PLUGINLIB_CATEGORY_CONTENT__behaviortree_cpp}share/${PROJECT_NAME}/${arg_TARGET}__btcpp_plugin_description.xml\n"
    PARENT_SCOPE
  )
  list(APPEND __PLUGINLIB_PLUGIN_CATEGORIES "behaviortree_cpp")
  set(__PLUGINLIB_PLUGIN_CATEGORIES "${__PLUGINLIB_PLUGIN_CATEGORIES}" PARENT_SCOPE)
endfunction()
