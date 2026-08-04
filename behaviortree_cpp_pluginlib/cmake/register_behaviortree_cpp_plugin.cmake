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
# register_behaviortree_cpp_plugin(my_library SUBTREES trees/patrol.xml trees/dock.xml)
#
# :param TARGET: name of a valid CMake shared library target to export plugins from
# :type TARGET: string
# :param SUBTREES: optional list of subtree XML files to ship with this plugin. Each file is
#   installed and registered so that BT::PluginAwareFactory loads it automatically, making its
#   <BehaviorTree ID="..."> definitions available to any loaded tree via <SubTree ID="..."/>.
# :type SUBTREES: list of files
#
# @public
#
function(register_behaviortree_cpp_plugin arg_TARGET)
  cmake_parse_arguments(ARG "" "" "SUBTREES" ${ARGN})
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

  # Ship subtree XML alongside this plugin, keyed by the (package-unique) target name.
  if(ARG_SUBTREES)
    _register_behaviortree_cpp_subtrees("${arg_TARGET}" ${ARG_SUBTREES})
  endif()
endfunction()

#
# Register BehaviorTree.CPP subtree XML files without a C++ plugin target.
#
# Use this for packages that ship reusable subtrees but build no node plugin library. The subtrees
# are installed and registered so that BT::PluginAwareFactory loads them automatically.
#
# Example usage:
# register_behaviortree_cpp_subtrees(FILES trees/patrol.xml trees/dock.xml)
# register_behaviortree_cpp_subtrees(NAME navigation FILES trees/patrol.xml)
#
# :param NAME: optional group suffix, used to build a unique resource marker. Defaults to "subtrees".
#   Pass distinct NAMEs when calling this more than once in a single package.
# :type NAME: string
# :param FILES: list of subtree XML files to install and register.
# :type FILES: list of files
#
# @public
#
function(register_behaviortree_cpp_subtrees)
  cmake_parse_arguments(ARG "" "NAME" "FILES" ${ARGN})
  if(NOT ARG_FILES)
    message(FATAL_ERROR "register_behaviortree_cpp_subtrees() called without FILES argument")
  endif()
  set(marker_suffix "subtrees")
  if(ARG_NAME)
    set(marker_suffix "${ARG_NAME}")
  endif()
  _register_behaviortree_cpp_subtrees("${marker_suffix}" ${ARG_FILES})
endfunction()

#
# Internal helper: install subtree XML files and register them in the "behaviortree_cpp_subtrees"
# ament resource index category for BT::PluginAwareFactory to discover at runtime.
#
# The marker is named "<PROJECT_NAME>__<marker_suffix>" so repeated calls in one package don't
# collide (ament_index_register_resource uses file(GENERATE), which errors on a reused path).
#
function(_register_behaviortree_cpp_subtrees marker_suffix)
  set(marker_content "")
  foreach(subtree_xml ${ARGN})
    get_filename_component(subtree_abs "${subtree_xml}" ABSOLUTE)
    if(NOT EXISTS "${subtree_abs}")
      message(FATAL_ERROR "register subtrees: file does not exist: ${subtree_abs}")
    endif()
    get_filename_component(subtree_name "${subtree_abs}" NAME)
    install(FILES "${subtree_abs}" DESTINATION share/${PROJECT_NAME}/behaviortree_subtrees)
    # Share-relative path including the package folder, resolved at runtime as <prefix>/share/<line>.
    string(APPEND marker_content "${PROJECT_NAME}/behaviortree_subtrees/${subtree_name}\n")
  endforeach()

  ament_index_register_resource("behaviortree_cpp_subtrees"
    CONTENT "${marker_content}"
    PACKAGE_NAME "${PROJECT_NAME}__${marker_suffix}"
  )
endfunction()
