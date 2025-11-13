# behaviortree_cpp_pluginlib

Pluginlib integration for registering and loading BehaviorTree.CPP extensions automatically.

This allows a workflow where any number of BT node plugins can be exposed in a distributed manner, with a single CMake call, so that no centralized plugin-exposure mechanism is needed to be kept updated.

The use of pluginlib enables discovering all these nodes automatic automatically at runtime, without having to provide a list ahead of time to the factory.

The packages in this repository are:
* [behaviortree_cpp_pluginlib](./behaviortree_cpp_pluginlib/) - main implementation, see here for all information
* [behaviortree_cpp_pluginlib_tests](./behaviortree_cpp_pluginlib_tests/) - separated test package to make sure all exposed build-system tools are working as intended for a user
