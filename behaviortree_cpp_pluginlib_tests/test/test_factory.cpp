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

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#include "behaviortree_cpp_pluginlib/factory.hpp"
#include "gtest/gtest.h"

TEST(Factory, AutofactoryEndToEnd)
{
  BT::PluginAwareFactory factory;
  const auto & builders = factory.builders();
  ASSERT_NO_THROW(builders.at("CustomNodeA1"));
  ASSERT_NO_THROW(builders.at("CustomNodeA2"));
  ASSERT_NO_THROW(builders.at("CustomNodeB1"));
  ASSERT_NO_THROW(builders.at("CustomNodeB2"));
  ASSERT_THROW(builders.at("NonexistentNode"), std::out_of_range);
}

TEST(Factory, SubtreesShippedByPluginsAreRegistered)
{
  BT::PluginAwareFactory factory;

  const auto trees = factory.registeredBehaviorTrees();
  // Shipped via the SUBTREES keyword on test_plugin_a.
  ASSERT_NE(std::find(trees.begin(), trees.end(), "SubtreeUsesA"), trees.end());
  // Shipped via the standalone register_behaviortree_cpp_subtrees() path.
  ASSERT_NE(std::find(trees.begin(), trees.end(), "StandaloneSubtree"), trees.end());

  // Each subtree instantiates directly: its concrete node was registered before it was loaded.
  ASSERT_NO_THROW(factory.createTree("SubtreeUsesA"));
  ASSERT_NO_THROW(factory.createTree("StandaloneSubtree"));
}

TEST(Factory, ShippedSubtreeIsUsableFromAnotherTree)
{
  BT::PluginAwareFactory factory;

  // A tree loaded later can reference the shipped subtree "for free" via <SubTree>.
  factory.registerBehaviorTreeFromText(
    R"(<root BTCPP_format="4">
         <BehaviorTree ID="Main">
           <SubTree ID="SubtreeUsesA"/>
         </BehaviorTree>
       </root>)");
  ASSERT_NO_THROW(factory.createTree("Main"));
}
