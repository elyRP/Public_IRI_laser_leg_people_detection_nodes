cmake_minimum_required(VERSION 3.8)
project(iri_perception_msgs)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()
## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_default_generators REQUIRED)
#find_package(rosidl_default_msgs REQUIRED)
#find_package(rosidl_default_interfaces REQUIRED)
find_package(rosidl_default_runtime REQUIRED)
#find_package(rosidl_generate_interfaces REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(action_msgs REQUIRED)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)

include_directories(
  include
)

## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()
set(msg_files
  "msg/Detection.msg"
  "msg/DetectionArray.msg"
  "msg/RestartSim.msg"
  "srv/GetDetections.srv"
  "srv/InitialiceSim.srv"
)

# Generate messages in the 'msg' folder  DEPENDENCIES rosidl_default_runtime std_msgs sensor_msgs geometry_msgs action_msgs
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/Detection.msg"
  "msg/DetectionArray.msg"
  "msg/RestartSim.msg"
  "srv/GetDetections.srv"
  "srv/InitialiceSim.srv"
  DEPENDENCIES std_msgs sensor_msgs geometry_msgs action_msgs
)

##ament_target_dependencies(${PROJECT_NAME}
##  rclcpp
##  rosidl_default_runtime
##)

# Export dependencies
ament_export_dependencies(rosidl_default_runtime std_msgs sensor_msgs geometry_msgs action_msgs)

#rosidl_generate_interfaces
# Install
install(DIRECTORY msg srv
  DESTINATION share/${PROJECT_NAME}
)

ament_package()

