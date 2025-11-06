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

#ifndef _laser_people_detection_alg_node_h_
#define _laser_people_detection_alg_node_h_

//#include <iri_base_algorithm/iri_base_algorithm.h>
#include "laser_people_detection_alg.h"

#include <pluginlib/class_list_macros.hpp> //Importante para librerias con .so

// [publisher subscriber headers]
//#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/msg/laser_scan.hpp>
//#include <visualization_msgs/MarkerArray.h>
#include <visualization_msgs/msg/marker_array.hpp>
#include <iri_perception_msgs/msg/detection_array.h>
#include "geometry_msgs/msg/pose_stamped.hpp"

// [service client headers]

// [action server client headers]

#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <memory>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <functional>
#include "rcl_interfaces/msg/set_parameters_result.hpp" 


#include "mutex.h"
#include "eventserver.h"

//#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/msg/laser_scan.hpp>
//#include <visualization_msgs/MarkerArray.h>
#include <visualization_msgs/msg/marker_array.hpp>
#include <iri_perception_msgs/msg/detection_array.hpp>

//include laser_people_detection_alg main library
#include "laserPeopleDetection.h"

using namespace std::chrono_literals;  // Para usar 100ms

/**
 * \brief IRI ROS Specific Algorithm Class
 *
 */
class LaserPeopleDetectionAlgNode : 
  public rclcpp::Node
 // public std::enable_shared_from_this<LaserPeopleDetectionAlgNode>
//public std::enable_shared_from_this<LaserPeopleDetectionAlgNode>,
//public algorithm_base::IriBaseAlgorithm<LaserPeopleDetectionAlgorithm>
{
  private:
    // [publisher attributes]
    //ros::Publisher peopleMarkers_array_publisher_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr peopleMarkers_array_publisher_;
    visualization_msgs::msg::MarkerArray MarkerArray_msg_;

    //ros::Publisher people_publisher_;
    rclcpp::Publisher<iri_perception_msgs::msg::DetectionArray>::SharedPtr people_publisher_;
    iri_perception_msgs::msg::DetectionArray detectionArray_msg_;

    // [subscriber attributes]
    //ros::Subscriber scan_subscriber_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_subscriber_;
    void scan_callback(const sensor_msgs::msg::LaserScan::ConstPtr& msg);
    CMutex scan_mutex_;
    
    rclcpp::TimerBase::SharedPtr timer_;


    // [service attributes]

    // [client attributes]

    // [action server attributes]

    // [action client attributes]

    // [class atributes]
    CEventServer * event_server_;
    std::string new_laser_event_id_;

    sensor_msgs::msg::LaserScan currentScan;

    //void init();
    void transformPoses(iri_perception_msgs::msg::DetectionArray & peoplePoses);
    void filterPoses(iri_perception_msgs::msg::DetectionArray & peoplePoses);
    void getPeopleMarkers(iri_perception_msgs::msg::DetectionArray & peopleList, visualization_msgs::msg::MarkerArray & people);

    double rangeThreshold;
    double detectionThreshold;
    double markerWidth;
    double markerHeight;
    double markerR;
    double markerG;
    double markerB;
    double markerA;

    bool filterPosesMode;
    double filterR;
    double filterXmin;
    double filterXmax;
    double filterYmin;
    double filterYmax;

    /*tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;
    std::string target_frame;*/
    // Member variables...
    tf2_ros::Buffer tf_buffer_; // Use shared pointer for buffer
    std::shared_ptr<tf2_ros::TransformListener>  tf_listener_; // Listener initialized using the buffer
    std::string target_frame;

    bool selectPosesFrame;
    bool selectScanFrame;
    std::string scanFrame;
    std::string posesFrame;
    
        /**
    * \brief Constructor
    *
    * This constructor initializes specific class attributes and all ROS
    * communications variables to enable message exchange.
    */
   
    
  public:
    //double rate_;
    LaserPeopleDetectionAlgNode(void);
    static std::shared_ptr<LaserPeopleDetectionAlgNode> create();
    void init();
   /**
    * \brief Destructor
    *
    * This destructor frees all necessary dynamic memory allocated within this
    * this class.
    */
    ~LaserPeopleDetectionAlgNode(void);

    //void iteration(const sensor_msgs::msg::LaserScan scan, iri_perception_msgs::msg::DetectionArray & people);
    //void setRangeThreshold(const float rangeThreshold);
    //void setDetectionThreshold(const float detectionThreshold);
    //void setBoostFilePaths(const std::string newPath, const std::string newPath2);
    //double getRate() const { return rate_; }
    
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
    
     std::shared_ptr<LaserPeopleDetectionAlgorithm> alg_;
    //CLaserPeopleDetection *myLaserPeopleDetection;
    
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
