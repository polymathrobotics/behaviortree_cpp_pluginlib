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

#include <memory>
#include <string>
#include <vector>

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
}

PluginAwareFactory::~PluginAwareFactory()
{
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
