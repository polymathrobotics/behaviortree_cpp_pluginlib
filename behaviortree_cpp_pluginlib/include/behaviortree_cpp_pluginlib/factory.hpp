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
#pragma once

#include <string>
#include <vector>

#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp_pluginlib/plugin.hpp"
#include "pluginlib/class_loader.hpp"

namespace BT
{

///
/// @class BT::PluginAwareFactory
/// @brief Factory specialization that automatically loads all available plugins on construction.
///
class PluginAwareFactory : public BT::BehaviorTreeFactory
{
public:
  /// @brief Construct a BehaviorTreeFactory with all available plugins loaded
  /// @param plugin_xmls Optional list of plugin XML files to load. If empty, will use pluginlib to discover plugins.
  explicit PluginAwareFactory(const std::vector<std::string> & plugin_xmls = {});
  virtual ~PluginAwareFactory();

private:
  pluginlib::ClassLoader<BT::BehaviorTreePlugin> loader_;
};

}  // namespace BT
