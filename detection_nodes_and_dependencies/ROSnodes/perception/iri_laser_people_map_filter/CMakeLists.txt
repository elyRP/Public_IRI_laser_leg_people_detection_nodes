cmake_minimum_required(VERSION 3.8)
project(iri_laser_people_map_filter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros REQUIRED)
find_package(iri_base_algorithm REQUIRED)
find_package(iri_perception_msgs REQUIRED)
find_package(rclcpp REQUIRED)
#find_package(rcl REQUIRED)
#find_package(rosidl_runtime_c REQUIRED)
#find_package(rosidl_typesupport_interface REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(iriutils REQUIRED)
find_package(pluginlib REQUIRED)
#find_package(diagnostic_msgs REQUIRED)
#find_package(dynamic_reconfigure REQUIRED)  # Dynamic reconfigure in ROS 2

# Include directories
include_directories(include
${rclcpp_INCLUDE_DIRS}
#${rcl_INCLUDE_DIRS}
${iriutils_INCLUDE_DIRS}
${laser_people_map_filter_INCLUDE_DIRS}
#${iri_base_algorithm_INCLUDE_DIRS}
#${rosidl_runtime_c_INCLUDE_DIRS}
#${rosidl_typesupport_interface_INCLUDE_DIRS}
#${diagnostic_msgs_INCLUDE_DIRS}
#${visualization_msgs_INCLUDE_DIRS}
${nav_msgs_INCLUDE_DIRS}
${iri_perception_msgs_INCLUDE_DIRS}
#${tf2_ros_INCLUDE_DIRS}
)

# Añade la ruta de la librería .so
link_directories(
  /usr/local/
)


# Generate dynamic reconfigure options
#generate_dynamic_reconfigure_options(cfg/LaserPeopleMapFilter.cfg)

# Declare a C++ library (if needed)
# add_library(${PROJECT_NAME} <list of source files>)

# Declare a C++ executable
add_executable(${PROJECT_NAME} src/laser_people_map_filter_alg.cpp src/laser_people_map_filter_alg_node.cpp)

# Link libraries
#target_link_libraries(${PROJECT_NAME} 
ament_target_dependencies(${PROJECT_NAME}
  ${iriutils_LIBRARIES}
  ${robot_aspsi_LIBRARIES}
  rclcpp 
  iri_base_algorithm 
  iri_perception_msgs 
  nav_msgs 
  visualization_msgs 
  tf2
  tf2_ros
  tf2_geometry_msgs
  iriutils
  pluginlib
)

# Add dynamic reconfigure dependencies
#add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS})

# Install
install(TARGETS ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch)

# Export package dependencies
ament_export_dependencies(rclcpp iri_base_algorithm iri_perception_msgs nav_msgs visualization_msgs tf2 tf2_ros iriutils)
ament_package()
