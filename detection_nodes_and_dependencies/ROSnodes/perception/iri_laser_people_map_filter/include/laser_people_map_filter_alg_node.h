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

#include <iri_base_algorithm/iri_base_algorithm.h>
#include "laser_people_map_filter_alg.h"

// [publisher subscriber headers]
#include <visualization_msgs/MarkerArray.h>
#include <nav_msgs/OccupancyGrid.h>
#include <iri_perception_msgs/detectionArray.h>

// [service client headers]

// [action server client headers]

#include <tf/transform_listener.h>
#include <queue>

/**
 * \brief IRI ROS Specific Algorithm Class
 *
 */
class LaserPeopleMapFilterAlgNode : public algorithm_base::IriBaseAlgorithm<LaserPeopleMapFilterAlgorithm>
{
  private:
    // [publisher attributes]
    ros::Publisher peopleMarkers_publisher_;
    visualization_msgs::MarkerArray MarkerArray_msg_;
    ros::Publisher mapFilteredPeople_publisher_;
    iri_perception_msgs::detectionArray detectionArray_msg_;

    // [subscriber attributes]
    ros::Subscriber map_subscriber_;
    void map_callback(const nav_msgs::OccupancyGrid::ConstPtr& msg);
    CMutex map_mutex_;
    ros::Subscriber people_subscriber_;
    void people_callback(const iri_perception_msgs::detectionArray::ConstPtr& msg);
    CMutex people_mutex_;

    // [service attributes]

    // [client attributes]

    // [action server attributes]

    // [action client attributes]

    tf::TransformListener tf_listener_;

    nav_msgs::OccupancyGrid map;
    //iri_perception_msgs::detectionArray people;
    iri_perception_msgs::detectionArray people_msg;
    
    std::queue<iri_perception_msgs::detectionArray> people_msg_queue;
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
    void getPeopleMarkers(iri_perception_msgs::detectionArray & peopleList, visualization_msgs::MarkerArray & people);

    Config config;
  public:
   /**
    * \brief Constructor
    * 
    * This constructor initializes specific class attributes and all ROS
    * communications variables to enable message exchange.
    */
    LaserPeopleMapFilterAlgNode(void);

   /**
    * \brief Destructor
    * 
    * This destructor frees all necessary dynamic memory allocated within this
    * this class.
    */
    ~LaserPeopleMapFilterAlgNode(void);

  protected:
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
    void node_config_update(Config &config, uint32_t level);

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
