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

#include "behaviortree_cpp/bt_factory.h"

namespace BT
{
/// @brief Interface class that lets us expose a pluginlib plugin with arbitrary BehaviorTree extensions.
/// Normally do not subclass directly, instead call BT_REGISTER_PLUGIN with a function body for register
class BehaviorTreePlugin
{
public:
  /// @brief Empty no-argument constructor for pluginlib loading.
  BehaviorTreePlugin() = default;

  /// @brief Abitrarily modify a factory, with the intention of adding new types as plugins.
  /// @param factory Factory to modify.
  virtual void registerTypes(BT::BehaviorTreeFactory & factory) = 0;
};
}  // namespace BT
