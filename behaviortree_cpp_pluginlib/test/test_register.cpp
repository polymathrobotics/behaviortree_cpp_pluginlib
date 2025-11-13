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
#include "polymath_test/catch2.hpp"

namespace btplugin::testing
{
class CustomNode1 : public BT::SyncActionNode
{
public:
  CustomNode1(const std::string & bt_node_name, const BT::NodeConfig & conf)
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

using CustomNode2 = CustomNode1;
using CustomNode3 = CustomNode2;

}  // namespace btplugin::testing

BT_PLUGIN_REGISTER_NODE(CustomNode1, btplugin::testing::CustomNode1)

BT_PLUGIN_REGISTER(factory)
{
  factory.registerNodeType<btplugin::testing::CustomNode2>("CustomNode2");
  factory.registerNodeType<btplugin::testing::CustomNode3>("CustomNode3");
}

TEST_CASE("Plugin registration macros")
{
  BT::BehaviorTreeFactory factory;

  auto plugin_fns = BT::get_plugin_register_functions();
  REQUIRE(plugin_fns.size() == 2);
  for (const auto & plugin_fn : plugin_fns) {
    plugin_fn(factory);
  }

  auto builders = factory.builders();
  REQUIRE_NOTHROW(builders.at("CustomNode1"));
  REQUIRE_NOTHROW(builders.at("CustomNode2"));
  REQUIRE_NOTHROW(builders.at("CustomNode3"));
  REQUIRE_THROWS_AS(builders.at("NonexistentNode"), std::out_of_range);
}
