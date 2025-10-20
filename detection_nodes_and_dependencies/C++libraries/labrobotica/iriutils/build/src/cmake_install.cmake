# Install script for directory: /home/noetic/ros_ws/src/labrobotica/iriutils/src

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/iriutils/libiriutils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/iriutils/libiriutils.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/iriutils/libiriutils.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/iri/iriutils" TYPE SHARED_LIBRARY FILES "/home/noetic/ros_ws/src/labrobotica/iriutils/lib/libiriutils.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/iriutils/libiriutils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/iriutils/libiriutils.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/iri/iriutils/libiriutils.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/iri/iriutils" TYPE FILE FILES
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/exceptions/exceptions.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/mutex/mutex.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/mutex/mutexexceptions.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/events/event.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/events/eventserver.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/events/eventexceptions.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/threads/thread.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/threads/threadserver.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/threads/threadexceptions.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/logs/log.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/logs/logexceptions.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/time/ctime.h"
    "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../include/time/ctimeexceptions.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/share/cmake-3.16/Modules/Findiriutils.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/share/cmake-3.16/Modules" TYPE FILE FILES "/home/noetic/ros_ws/src/labrobotica/iriutils/src/../Findiriutils.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/noetic/ros_ws/src/labrobotica/iriutils/build/src/examples/cmake_install.cmake")

endif()

