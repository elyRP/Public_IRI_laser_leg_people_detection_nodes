laser_people_detection library                         {#mainpage}
============

## Description

C++ library to detect people using a Boosting based classifier. 
The person's leg data is coming from a 2D range lidar. 
An already trained classifier is provided in `/data/classifierData/boostFile.txt`, based on a dataset using a Hokuyo UTM-30LX sensor at ~0.4m height from the ground. 

See its ROS wrapper here: [iri_laser_people_detection](https://gitlab.iri.upc.edu/labrobotica/ros/perception/iri_laser_people_detection)

## Installation

* Add the labrobotica repository if it is not already added:

    Run the commands on _add repository_ and _add key_ from [labrobotica_how_to installation](https://gitlab.iri.upc.edu/labrobotica/labrobotica_how_to/-/blob/master/README.md#installation)


* Install the package:


      sudo apt update && sudo apt install iri-laser-people-detection-dev




## For developers

<details><summary>click here</summary>
<p>

## Dependencies

This package requires of the following system libraries and packages

* [cmake](https://www.cmake.org "CMake's Homepage"), a cross-platform build system.
* [doxygen](http://www.doxygen.org "Doxygen's Homepage") and [graphviz](http://www.graphviz.org "Graphviz's Homepage") to generate the documentation.

Under linux all of these utilities are available in ready-to-use packages.

This package also requires of the following IRI libraries:

* [iriutils](https://gitlab.iri.upc.edu/labrobotica/algorithms/iriutils "iriutils gitlab page"), a set of basic tools.

## Compilation and installation from source

Clone this repository and create a build folder inside:

    mkdir build

Inside the build folder execute the following commands:

    cmake ..

The default build mode is DEBUG. That is, objects and executables include debug information.

The RELEASE build mode optimizes for speed. To build in this mode execute instead
      cmake .. -DCMAKE_BUILD_TYPE=RELEASE      

The release mode will be kept until next time cmake is executed.

    make -j $(nproc)

In case no errors are reported, the generated libraries (if any) will be located at the
_lib_ folder and the executables (if any) will be located at the _bin_ folder.

In order to be able to use the library, it it necessary to copy it into the system.
To do that, execute

    make install

as root and the shared libraries will be copied to */usr/local/lib/iri/laser_people_detection* directory
and the header files will be copied to */usr/local/include/iri/laser_people_detection* directory.
At this point, the library may be used by any user.

To remove the library from the system, exceute

    make uninstall

as root, and all the associated files will be removed from the system.

To generate the documentation execute the following command:

    make doc

## How to use it

To use this library in an other library or application, in the CMakeLists.txt file, first it is necessary to locate if the library has been installed or not using the following command

    FIND_PACKAGE(laser_people_detection)

In the case that the package is present, it is necessary to add the header files directory to the include directory path by using

    INCLUDE_DIRECTORIES(${laser_people_detection_INCLUDE_DIRS})

and it is also necessary to link with the desired libraries by using the following command

    TARGET_LINK_LIBRARIES(<executable name> ${laser_people_detection_LIBRARIES})

## Examples

* **laser_people_detection_test**: A test example outputs the detected people using sample data from a local logFile.txt

      cd build 
      ../bin/laser_people_detection_test

  * Partial terminal output:
  
        --- Person = ( x, y, probability, covariance )
        ------- it: 0 -------
           ( -0.175332 -0.870649 0.245762 0.00409358 )
           ( 3.85127 -0.225226 0.02209 0.00307464 )
           ( -0.598148 1.18134 0.188613 0.00492409 )
   
* **laser_people_label_assembler_test**: uses 2 sets of labeled files (params/points/labels.txt) and generates 1 set of labeled files

      cd build
      ../bin/laser_people_label_assembler_test

</p>
</details>
