cmake_minimum_required(VERSION 3.8)
project(iri_laser_people_detection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


# Find ament_cmake and other required packages
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros REQUIRED)
find_package(rclcpp REQUIRED)
find_package(iri_base_algorithm REQUIRED)
find_package(iri_perception_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(iriutils REQUIRED)
find_package(laser_people_detection REQUIRED)
find_package(rclcpp REQUIRED)
find_package(pluginlib REQUIRED)
#find_library(laser_people_detection liblaser_people_detection.so)

#set_target_properties(mi_nodo PROPERTIES
#  INSTALL_RPATH "~/ros2_ws/src/codeSubjectMobileRobots/install/iri_laser_people_detection/lib"
#)
# Include directories
include_directories(
  include
  ${laser_people_detection_INCLUDE_DIRS}
  ${iriutils_INCLUDE_DIRS}
  /usr/local/lib/iri/laser_people_detection/
)



add_library(laser_people_detection SHARED ~/ros2_ws/src/codeSubjectMobileRobots/labrobotica/laser_people_detection/src/laserPeopleDetection.cpp )
target_include_directories(laser_people_detection PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
)

# Añade la ruta de la librería .so
link_directories(
  /usr/local/
)

 #  install(FILES
 #  lib/libiriutils.so
  #   DESTINATION lib/iri_laser_people_detection
 #  )

#install(TARGETS iriutils
#  LIBRARY DESTINATION lib
#)
# Declare a cpp executable
add_executable(${PROJECT_NAME} src/laser_people_detection_alg.cpp src/laser_people_detection_alg_node.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(${PROJECT_NAME} PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

target_link_libraries(${PROJECT_NAME}
  ${iriutils_LIBRARIES}
  ${laser_people_detection_LIBRARIES}
  
 # ${PROJECT_SOURCE_DIR}/laser_people_detection/lib/liblaser_people_detection.so
)

# Link libraries
ament_target_dependencies(${PROJECT_NAME}
  iri_base_algorithm
  iri_perception_msgs
  sensor_msgs
  visualization_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
  pluginlib
  iriutils
  rclcpp
  laser_people_detection
)



set(lpd_PATH "${PROJECT_SOURCE_DIR}/laser_people_detection/lib/liblaser_people_detection.so")
# Dynamic reconfigure is not directly supported in ROS 2, so you may need to implement dynamic parameter updates manually

install(FILES ${lpd_PATH} DESTINATION lib/${PROJECT_NAME})
#  lib/iri_laser_people_detection/iriutils.so
#  DESTINATION lib
#)


# Install targets
 #install(TARGETS ${PROJECT_NAME}
 #  EXPORT export_laser_people_detection
 #  LIBRARY DESTINATION lib/${PROJECT_NAME}
 # INCLUDES DESTINATION include/${PROJECT_NAME}
 #)
 
 install(TARGETS laser_people_detection
  EXPORT export_laser_people_detection
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS iri_laser_people_detection
 RUNTIME DESTINATION lib/${PROJECT_NAME}
 LIBRARY DESTINATION lib/${PROJECT_NAME}
 ARCHIVE DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

#ament_target_dependencies(iri_laser_people_detection iriutils)
ament_export_targets(export_laser_people_detection)
ament_export_include_directories(include)
ament_export_libraries(laser_people_detection)

# Install launch files, if any
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})



ament_package()
