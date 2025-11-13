# Implementation Details

## BT Plugin Base Class

[plugin.hpp](./include/behaviortree_cpp_pluginlib/plugin.hpp) defines the abstract base class `BT::BehaviorTreePlugin` which has a no-argument constructor and so can be exposed to `pluginlib`.

Prerequisites for `pluginlib`:
1. Plugins must derive from a known base class and be able to be used polymorphically
1. Plugins must provide a no-argument constructor

## C++ Registration Macros

In [register_macro.hpp](./include/behaviortree_cpp_pluginlib/register_macro.hpp) we provide the macros needed to register an arbitrary number of plugin entrypoints from a single library.

See the doc comments in that file for the breakdown of the implementation.

## CMake Macro

Follow the logic in [register_behaviortree_cpp_plugin.cmake](./cmake/register_behaviortree_cpp_plugin.cmake) for the implementation details. High level:
1. Add `autoregistrar.cpp` to the library target
1. Create `plugin_description.xml` file and register it with the resource index for `pluginlib` to find
