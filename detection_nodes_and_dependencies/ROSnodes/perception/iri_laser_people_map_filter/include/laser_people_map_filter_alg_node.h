// Copyright (C) 2010-2011 Institut de Robotica i Informatica Industrial, CSIC-UPC.
// Author 
// All rights reserved.
//
// This file is part of iri-ros-pkg
// iri-ros-pkg is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// IMPORTANT NOTE: This code has been generated through a script from the 
// iri_ros_scripts. Please do NOT delete any comments to guarantee the correctness
// of the scripts. ROS topics can be easly add by using those scripts. Please
// refer to the IRI wiki page for more information:
// http://wikiri.upc.es/index.php/Robotics_Lab

#ifndef _laser_people_map_filter_alg_node_h_
#define _laser_people_map_filter_alg_node_h_

#include <iostream>
#include <string>

// new libs ini:
#include <pluginlib/class_list_macros.hpp> //Importante para librerias con .so
#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <functional>
#include "rcl_interfaces/msg/set_parameters_result.hpp" 
#include "mutex.h"
#include "eventserver.h"
using namespace std::chrono_literals;  // Para usar 100ms
// new libs fin

#include <iri_base_algorithm/iri_base_algorithm.h>
#include "laser_people_map_filter_alg.h"

// [publisher subscriber headers]
#include <visualization_msgs/msg/marker_array.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <iri_perception_msgs/msg/detection_array.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

// [service client headers]

// [action server client headers]

//#include <tf/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

//#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <queue>

/**
 * \brief IRI ROS Specific Algorithm Class
 *
 */
class LaserPeopleMapFilterAlgNode : public rclcpp::Node 
//public algorithm_base::IriBaseAlgorithm<LaserPeopleMapFilterAlgorithm>
{
  private:
    // new variables ros2:
    rclcpp::TimerBase::SharedPtr timer_;
     // [class atributes]
    CEventServer * event_server_;
    std::string new_map_event_id_;
    //new variables ros2 fin
  
    // [publisher attributes]
    //ros::Publisher peopleMarkers_publisher_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr peopleMarkers_publisher_;
    visualization_msgs::msg::MarkerArray MarkerArray_msg_;
    //ros::Publisher mapFilteredPeople_publisher_;
    rclcpp::Publisher<iri_perception_msgs::msg::DetectionArray>::SharedPtr mapFilteredPeople_publisher_;

    iri_perception_msgs::msg::DetectionArray detectionArray_msg_;

    // [subscriber attributes]
    //ros::Subscriber map_subscriber_;
    rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr map_subscriber_;
    void map_callback(const nav_msgs::msg::OccupancyGrid::ConstPtr& msg);
    CMutex map_mutex_;
    //ros::Subscriber people_subscriber_;
    rclcpp::Subscription<iri_perception_msgs::msg::DetectionArray>::SharedPtr people_subscriber_;
    void people_callback(const iri_perception_msgs::msg::DetectionArray::ConstPtr& msg);
    CMutex people_mutex_;

    // [service attributes]

    // [client attributes]

    // [action server attributes]

    // [action client attributes]

    //tf::TransformListener tf_listener_;
    tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;


    nav_msgs::msg::OccupancyGrid map;
    //iri_perception_msgs::detectionArray people;
    iri_perception_msgs::msg::DetectionArray people_msg;
    
    std::queue<iri_perception_msgs::msg::DetectionArray> people_msg_queue;
    bool map_received;
    bool people_received;
    double neighborRadius;

    double markerWidth;
    double markerHeight;
    double markerR;
    double markerG;
    double markerB;
    double markerA;

    std::string map_frame;

    bool checkCellNearObstacle(int index, int neighborDistance);
    void getPeopleMarkers(iri_perception_msgs::msg::DetectionArray & peopleList, visualization_msgs::msg::MarkerArray & people);

    //Config config;
  public:
   /**
    * \brief Constructor
    * 
    * This constructor initializes specific class attributes and all ROS
    * communications variables to enable message exchange.
    */
    LaserPeopleMapFilterAlgNode(void);
    static std::shared_ptr<LaserPeopleMapFilterAlgNode> create();
   /**
    * \brief Destructor
    * 
    * This destructor frees all necessary dynamic memory allocated within this
    * this class.
    */
    ~LaserPeopleMapFilterAlgNode(void);


 /**
    * \brief main node thread
    *
    * This is the main thread node function. Code written here will be executed
    * in every node loop while the algorithm is on running state. Loop frequency 
    * can be tuned by modifying loop_rate attribute.
    *
    * Here data related to the process loop or to ROS topics (mainly data structs
    * related to the MSG and SRV files) must be updated. ROS publisher objects 
    * must publish their data in this process. ROS client servers may also
    * request data to the corresponding server topics.
    */
    void mainNodeThread(void);
    
     std::shared_ptr<LaserPeopleMapFilterAlgorithm> alg_;
     
  protected:
  

   /**
    * \brief dynamic reconfigure server callback
    * 
    * This method is called whenever a new configuration is received through
    * the dynamic reconfigure. The derivated generic algorithm class must 
    * implement it.
    *
    * \param config an object with new configuration from all algorithm 
    *               parameters defined in the config file.
    * \param level  integer referring the level in which the configuration
    *               has been changed.
    */
    //void node_config_update(Config &config, uint32_t level);

   /**
    * \brief node add diagnostics
    *
    * In this abstract function additional ROS diagnostics applied to the 
    * specific algorithms may be added.
    */
    void addNodeDiagnostics(void);

    // [diagnostic functions]
    
    // [test functions]
};

#endif
