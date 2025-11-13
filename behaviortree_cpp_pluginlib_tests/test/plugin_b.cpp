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

#include <string>

#include "behaviortree_cpp_pluginlib/register_macro.hpp"

namespace testing::plugin_b
{

class CustomNodeB1 : public BT::SyncActionNode
{
public:
  CustomNodeB1(const std::string & bt_node_name, const BT::NodeConfig & conf)
  : BT::SyncActionNode(bt_node_name, conf)
  {}

  static BT::PortsList providedPorts()
  {
    return BT::PortsList{};
  }

  BT::NodeStatus tick() override
  {
    return BT::NodeStatus::SUCCESS;
  }
};

using CustomNodeB2 = CustomNodeB1;

}  // namespace testing::plugin_b

BT_PLUGIN_REGISTER(factory)
{
  factory.registerNodeType<testing::plugin_b::CustomNodeB1>("CustomNodeB1");
  factory.registerNodeType<testing::plugin_b::CustomNodeB2>("CustomNodeB2");
}
