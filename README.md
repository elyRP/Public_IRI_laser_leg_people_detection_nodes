### Public_IRI_laser_leg_people_detection_nodes
These algoriths were created at IRI for the TFC of Fernando Herrero Cotarelo. The migred version for ROS2 humble is created by Ely Repiso. It is included here because is used in the Adaptive Social Planner to acompani people and includes licences and recognition from the initial people, institutions and repositories.
The code is under the licence CC BY-NC-ND 4.0 License from the UPC commons documents.

Also, sorry. It is a problem to upload the ROS nodes of this package. I am trying to correct it today, 06/11/2025. If I cannot, I will do it as soon as possible.

Notice: UPC publishes undergraduate, master's theses and PhD dissertations in this way (under license CC By-NC-ND 4.0). However, you can use it for free, but for commercial use or modifications, you should contact me to be able to do it. However, if collaborations to create derivative works or commercial works become overwhelming to me, or if we need to help you incorporate parts of these works into your own, or if there are too many agreements to include them with companies and I can't help everyone collaborating with them, I will speak with the legal department of UPC to see how we can release the code to 100% open source. But in between this is like open source + including support for sure (not like normal open source that they do not support you to combine the works). Then, this actual situation should benefit more both parties, you and me.

## Laser People Detection
# Author recognition and citation:

This code was created at the IRI institution, website: https://www.iri.upc.edu/
It was authored by Fernando Herrero and published as TFC in UPC, with name: Detección automática de personas mediante láser 2D y su aplicación a la robótica de servicios
link: https://upcommons.upc.edu/entities/publication/a048cb04-9d46-4742-8cfa-bd6bbebbf44a
Notice: Sorry, this is only in spanish.

The migration of these nodes to ROS2-humble is performed by Ely Repiso. And also the migration to Noetic for the notes included here, but I am not sure if it is included also in any repository of the IRI, that I can not find and access right now.

The compatibility with the Noetic version of the Adaptive Social Planner to accompany people (one individual or a group of people, of the approaching versions). It is only granted if you use this provided package, not if you use the nodes included in IRI repositories. Then, to be sure that you can use it well with the ASP download and install this version (NOT the IRI ones). If you use the older or other versions, it will not be possible to get any support.

IMPORTANT: If you use it, please cite the IRI institution and give the authorship of the original versions to Fernando Herrero and cite his TFC document, and for the migrations to Ely Repiso and this repository where you download the code.

iri_laser_people_detection (ROS package) {#mainpage}
============

# Description

ROS wrapper of the [laser_people_detection](https://gitlab.iri.upc.edu/labrobotica/algorithms/laser_people_detection) C++ driver library.

The node inherits from the [iri_base_algorithm](https://gitlab.iri.upc.edu/labrobotica/ros/iri_core/iri_base_algorithm) class.

# ROS Interface

### Topic subscribers

- *~/scan* ([sensor_msgs/LaserScan](http://docs.ros.org/noetic/api/sensor_msgs/html/msg/LaserScan.html)): 
  messages coming from an horizontal laser scan sensor

### Topic publishers

- *~/people* ([iri_perception_msgs::detectionArray](https://gitlab.iri.upc.edu/labrobotica/ros/perception/iri_perception_msgs/-/blob/master/msg/detectionArray.msg)):
  people detection messages

- *~/markers* ([visualization_msgs::MarkerArray](http://docs.ros.org/noetic/api/visualization_msgs/html/msg/MarkerArray.html)):
  visualization markers to be displayed in Rviz

# Dependencies

This node has the following dependencies:

 * ROS
     * tf
     * sensor_msgs
     * visualization_msgs
 * IRI-ROS
     * [iri_base_algorithm](https://gitlab.iri.upc.edu/labrobotica/ros/iri_core/iri_base_algorithm) Note by Ely: for ROS2-humble I think that I removed this dependenci to be more easy the integration. If you do not find here, you do not neet to install.
     * [iri_perception_msgs](https://gitlab.iri.upc.edu/labrobotica/ros/perception/iri_perception_msgs)
 * IRI
     * [iriutils](https://gitlab.iri.upc.edu/labrobotica/algorithms/iriutils)
     * [laser_people_detection](https://gitlab.iri.upc.edu/labrobotica/algorithms/laser_people_detection)

# Install

Install its dependencies:
* ROS dependencies can be installed with `sudo apt install ros-$ROS_DISTRO-dependency-name`
* IRI-ROS dependencies normally need to be cloned and compiled in an active ROS workspace, as explained in their README file.
* IRI dependencies can be installed as debian packages, or cloned, compiled and installed from sources. See their README file for details.

This package can be installed by cloning the repository inside an active ROS workspace:

```
roscd
cd ../src
git clone https://gitlab.iri.upc.edu/labrobotica/ros/perception/iri_laser_people_detection.git 
```

However, this package is normally used as part of a wider installation (i.e. a 
robot, an experiment or a demosntration) which will normally include a complete 
rosinstall file to be used with the [wstool](http://wiki.ros.org/wstool) tool.

# How to use it

Example of use playing a rosbag, running the node, showing detections on Rviz and reconfigurable parameters on rqt_reconfigure.

`roslaunch iri_laser_people_detection test.launch`

![](doc/images/rviz.png)



