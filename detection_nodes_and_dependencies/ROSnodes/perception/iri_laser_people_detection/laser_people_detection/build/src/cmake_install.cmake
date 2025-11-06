# Install script for directory: /home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "DEBUG")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/laser_people_detection/liblaser_people_detection.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/laser_people_detection/liblaser_people_detection.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/laser_people_detection/liblaser_people_detection.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/iri/laser_people_detection" TYPE SHARED_LIBRARY FILES "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/build/lib/liblaser_people_detection.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/laser_people_detection/liblaser_people_detection.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/laser_people_detection/liblaser_people_detection.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/laser_people_detection/liblaser_people_detection.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/iri/laser_people_detection" TYPE FILE FILES
    "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src/../include/laserPeopleCommon.h"
    "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src/../include/laserPeopleDetection.h"
    "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src/../include/laserPeopleLabel.h"
    "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src/boostsrc/BoostedCommittee.h"
    "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src/boostsrc/SPHypothesis.h"
    "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src/boostsrc/IWeakHypothesis.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lpd/boostData" TYPE FILE FILES
    "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src/../data/classifierData/boostFile.txt"
    "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src/../data/classifierData/boostFile2.txt"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/share/cmake-3.22/Modules/Findlaser_people_detection.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/share/cmake-3.22/Modules" TYPE FILE FILES "/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_detection/laser_people_detection/src/../Findlaser_people_detection.cmake")
endif()

