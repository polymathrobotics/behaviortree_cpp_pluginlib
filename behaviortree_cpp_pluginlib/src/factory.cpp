// Copyright (c) 2025-present Polymath Robotics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "behaviortree_cpp_pluginlib/factory.hpp"

#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ament_index_cpp/get_resource.hpp"
#include "ament_index_cpp/get_resources.hpp"
#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp_pluginlib/plugin.hpp"
#include "rcutils/logging_macros.h"

namespace BT
{

PluginAwareFactory::PluginAwareFactory(const std::vector<std::string> & plugin_xmls)
: BT::BehaviorTreeFactory()
, loader_("behaviortree_cpp", "BT::BehaviorTreePlugin", "plugin", plugin_xmls)
{
  RCUTILS_LOG_INFO_NAMED("behaviortree_cpp_pluginlib", "Created classloader for BT::BehaviorTreePlugin");
  for (const std::string & class_name : loader_.getDeclaredClasses()) {
    std::shared_ptr<BT::BehaviorTreePlugin> plugin = loader_.createSharedInstance(class_name);
    std::string class_library_path = loader_.getClassLibraryPath(class_name);
    RCUTILS_LOG_INFO_NAMED(
      "behaviortree_cpp_pluginlib",
      "Loaded plugin class %s from library %s",
      class_name.c_str(),
      class_library_path.c_str());
    plugin->registerTypes(*this);
  }

  // Phase 2: now that every node type from every discovered plugin has been registered above,
  // load any subtree XML files shipped by plugin packages. Ordering matters: BT.CPP runs its
  // XML verification at registration time and throws if a subtree references a concrete node
  // type that is not yet registered, so this must happen strictly after the loop above.
  // Subtrees are discovered via the "behaviortree_cpp_subtrees" ament resource index category,
  // populated by the register_behaviortree_cpp_subtrees() CMake helper.
  const std::string subtree_resource = "behaviortree_cpp_subtrees";
  for (const auto & [marker_name, install_prefix] :
    ament_index_cpp::get_resources(subtree_resource))
  {
    std::string content;
    if (!ament_index_cpp::get_resource(subtree_resource, marker_name, content)) {
      continue;
    }
    // Content is a newline-separated list of share-relative paths, each already including the
    // providing package's folder (e.g. "my_pkg/behaviortree_subtrees/foo.xml"). The install
    // prefix comes from the resource itself, so this resolves correctly under both merged and
    // isolated colcon installs.
    std::istringstream stream(content);
    std::string relative_path;
    while (std::getline(stream, relative_path)) {
      if (relative_path.empty()) {
        continue;  // A trailing newline yields an empty final token.
      }
      const std::filesystem::path subtree_path =
        std::filesystem::path(install_prefix) / "share" / relative_path;
      try {
        registerBehaviorTreeFromFile(subtree_path);
        RCUTILS_LOG_INFO_NAMED(
          "behaviortree_cpp_pluginlib",
          "Registered subtree(s) from %s",
          subtree_path.c_str());
      } catch (const std::exception & e) {
        // Keep the factory usable even if one package ships a malformed subtree or one that
        // references a node no loaded plugin provides. Log loudly and skip it.
        RCUTILS_LOG_ERROR_NAMED(
          "behaviortree_cpp_pluginlib",
          "Failed to register subtree from %s: %s",
          subtree_path.c_str(),
          e.what());
      }
    }
  }
}

PluginAwareFactory::~PluginAwareFactory()
{
  // Clear registered tree definitions first. These hold only parsed XML owned by the
  // behaviortree_cpp library (not by any dlopen'd plugin), so this is defensive/symmetric
  // rather than strictly required for lifetime correctness.
  clearRegisteredBehaviorTrees();

  // First grab all the IDs, since unregistering them modifies the map and invalidates iterators
  std::vector<std::string> ids_to_unregister;
  for (const auto & [id, _] : builders()) {
    ids_to_unregister.push_back(id);
  }
  // Now unregister all builders from the underlying factory, to make sure objects from classloader
  // loaded libraries are destroyed before the classloader itself is destroyed.
  for (const auto & id : ids_to_unregister) {
    try {
      unregisterBuilder(id);
    } catch (const BT::LogicError & e) {
      // Thrown when trying to unregister builtin IDs, but that's fine, just skip it.
    }
  }
}

}  // namespace BT
