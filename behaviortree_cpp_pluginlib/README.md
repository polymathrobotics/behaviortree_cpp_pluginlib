# BehaviorTree.CPP Plugins

This package provides a set of tools that allows users to easily export plugins for `behaviortree_cpp`, such as new types of BT Nodes.

Under the hood, it uses ROS' `pluginlib` to allow for discovery of plugins without hardcoding of library names and classes - which the patterns provided by `behaviortree_cpp` and `navigation2` currently require.

# Usage

There are two usage patterns - the plugin provider and the plugin consumer.

## Registering Plugins

Simple as 1, 2, 3: depend on this package, register your library as a plugin provider, call registration macro on plugin classes.

1. `package.xml` - you need only a build dependency on this package

    ```xml
    ...
    <build_depend>behaviortree_cpp_pluginlib</build_depend>
    ...
    ```

2. `CMakeLists.txt` - find the package, link against its library target, and call custom macro to register your target as containing plugins

    ```cmake
    find_package(behaviortree_cpp_pluginlib REQUIRED)

    ...
    add_library(my_plugin_library
      src/source1.cpp
      ...
    )
    target_link_libraries(my_plugin_library
      behaviortree_cpp_pluginlib::plugin
      ...
    )
    register_behaviortree_cpp_plugin(my_plugin_library)
    ...
    ```

3. C++ sources (note: registration needs to happen in compilation unit, not in exposed header!)

    ```c++
    #include "behaviortree_cpp_pluginlib/register_macro.hpp

    ...

    namespace my_namespace
    {
    class MyActionNodeClass : public BT::SyncActionNode
    {
      ...
    };
    }  // namespace my_namespace

    ...

    BT_REGISTER_PLUGIN_NODE(MyActionNodeName, my_namespace::MyActionNodeClass)
    ```

4. Also note a more generic exposure mode. `BT_REGISTER_PLUGIN_NODE` is provided for simple use in the average case, but if you have other exposures to do, you can use:

    ```
    BT_PLUGIN_REGISTER(factory)
    {
      // factory will be a BT::BehaviorTreeFactory &
      factory.ArbitraryMethodCalls();
    }
    ```

## Loading Plugins

To load all registered plugins, link against the exported library target and use the `BT::PluginAwareFactory`

1. `CMakeLists.txt` - find and link against the loader

    ```cmake
    target_link_libraries(my_loader_lib
      behaviortree_cpp_pluginlib::behaviortree_cpp_pluginlib
      ...
    )
    ```

2. Create your factory and load all plugins with the provided function:

    ```c++
    #include "behaviortree_cpp_pluginlib/factory.hpp"
    ...
      BT::PluginAwareFactory factory;
    ...
    ```

# Implementation Details

For more information about what's happening under the hood to enable these usage patterns, see [DEVELOPING.md](./DEVELOPING.md)
