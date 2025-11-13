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

#include <fstream>
#include <string>
#include <vector>

#include "behaviortree_cpp_pluginlib/factory.hpp"
#include "polymath_test/catch2.hpp"

TEST_CASE("Autofactory finds plugins registered via the various macros")
{
  BT::PluginAwareFactory factory;
  const auto & builders = factory.builders();
  REQUIRE_NOTHROW(builders.at("CustomNodeA1"));
  REQUIRE_NOTHROW(builders.at("CustomNodeA2"));
  REQUIRE_NOTHROW(builders.at("CustomNodeB1"));
  REQUIRE_NOTHROW(builders.at("CustomNodeB2"));
  REQUIRE_THROWS_AS(builders.at("NonexistentNode"), std::out_of_range);
}
