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

## Registering Subtrees

You can also ship reusable subtrees as `.xml` files. They are discovered and registered by `BT::PluginAwareFactory` automatically, so any loaded tree can reference them via `<SubTree ID="..."/>` without loading files by hand.

1. Write a subtree XML with a single `<BehaviorTree ID="...">` (the `ID` is the name you reference as a `<SubTree>`):

    ```xml
    <root BTCPP_format="4">
      <BehaviorTree ID="GoAndBeep">
        ...
      </BehaviorTree>
    </root>
    ```

2. `CMakeLists.txt` - register the file(s). Use the `SUBTREES` keyword to ship them alongside a plugin library (e.g. one that provides the nodes the subtree uses):

    ```cmake
    register_behaviortree_cpp_plugin(my_plugin_library
      SUBTREES trees/go_and_beep.xml
    )
    ```

    Or, for a package that ships subtrees but builds no plugin library, use the standalone function:

    ```cmake
    register_behaviortree_cpp_subtrees(FILES trees/go_and_beep.xml)
    # Pass a distinct NAME when calling more than once in a single package:
    # register_behaviortree_cpp_subtrees(NAME navigation FILES trees/go_and_beep.xml)
    ```

Note: a subtree may only reference built-in nodes or nodes provided by a loaded plugin. Referencing a node that is registered manually after the factory is constructed is not supported — such a subtree is logged and skipped at load time.

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

3. All registered nodes _and_ subtrees are now available. Reference a shipped subtree by its `ID` from any tree you load - no need to load its file yourself:

    ```c++
    factory.registerBehaviorTreeFromText(R"(
      <root BTCPP_format="4">
        <BehaviorTree ID="Main">
          <Sequence>
            <SubTree ID="GoAndBeep"/>
          </Sequence>
        </BehaviorTree>
      </root>)");

    auto tree = factory.createTree("Main");
    tree.tickWhileRunning();
    ```

# Implementation Details

For more information about what's happening under the hood to enable these usage patterns, see [DEVELOPING.md](./DEVELOPING.md)
