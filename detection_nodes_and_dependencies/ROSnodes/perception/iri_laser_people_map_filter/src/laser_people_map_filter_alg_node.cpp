#include "laser_people_map_filter_alg_node.h"

LaserPeopleMapFilterAlgNode::LaserPeopleMapFilterAlgNode(void) :
  Node("laser_people_map_filter_node"),
  //algorithm_base::IriBaseAlgorithm<LaserPeopleMapFilterAlgorithm>(),
  tf_buffer_(this->get_clock()),  // Initialize tf_buffer_ with the node's clock
  tf_listener_(tf_buffer_)        // Initialize tf_listener_ with the tf_buffer_
{
 RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-map-filter: Inicio creador nodo step1 ");
  //init class attributes if necessary
  //this->setRate(100);//in [Hz]
   alg_ = std::make_shared<LaserPeopleMapFilterAlgorithm>();
   
  // RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 2 ");
  using std::placeholders::_1;  // se puede poner dentro del constructor o arriba del todo
  // RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 3 ");
  auto callback = std::bind(&LaserPeopleMapFilterAlgNode::mainNodeThread, this);
  //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 4 ");
  timer_ = this->create_wall_timer(20ms, callback);

  this->map_received=false;
  this->people_received=false;
  //this->neighborRadius = 0.25; //m
  this->declare_parameter<double>("neighborRadius", 0.25); //m

  this->markerWidth=0.5;
  this->markerHeight=0.2;
  this->markerR=1.0;
  this->markerG=1.0;
  this->markerB=0.0;
  this->markerA=0.75;

  this->map_frame="odom";

  //this->public_node_handle_.getParam("neighborRadius", this->neighborRadius);
  this->get_parameter("neighborRadius", this->neighborRadius);

  // [init publishers]
  //this->peopleMarkers_publisher_ = this->public_node_handle_.advertise<visualization_msgs::msg::MarkerArray>("markers", 1);
  this->peopleMarkers_publisher_ = this->create_publisher<visualization_msgs::msg::MarkerArray>("/markersPeoplefromFilter", 1);
  //this->mapFilteredPeople_publisher_ = this->public_node_handle_.advertise<iri_perception_msgs::msg::DetectionArray>("people_out", 1);
  this->mapFilteredPeople_publisher_ = this->create_publisher<iri_perception_msgs::msg::DetectionArray>("/people_filtered_out", 1);
  
  // [init subscribers]
  //this->map_subscriber_ = this->public_node_handle_.subscribe("map", 1, &LaserPeopleMapFilterAlgNode::map_callback, this);
  this->map_subscriber_ = this->create_subscription<nav_msgs::msg::OccupancyGrid>("/map", 10, std::bind(&LaserPeopleMapFilterAlgNode::map_callback, this, std::placeholders::_1));
  //this->people_subscriber_ = this->public_node_handle_.subscribe("people", 1, &LaserPeopleMapFilterAlgNode::people_callback, this);
  this->people_subscriber_ = this->create_subscription<iri_perception_msgs::msg::DetectionArray>("/people", 1, std::bind(&LaserPeopleMapFilterAlgNode::people_callback, this, std::placeholders::_1));
  
  // [init services]
  
  // [init clients]
  
  // [init action servers]
  
  // [init action clients]
  
  RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-map-filter: FIN creador nodo step2 ");
}

LaserPeopleMapFilterAlgNode::~LaserPeopleMapFilterAlgNode(void)
{
  // [free dynamic memory]
}

void LaserPeopleMapFilterAlgNode::mainNodeThread(void)
{
  alg_->lock();
  bool publish=false;
  
  //RCLCPP_INFO(get_logger(), "mainNodeThread 1");
 // std::cout << "map_received"<<map_received << std::endl;
  
  iri_perception_msgs::msg::DetectionArray people;
  rclcpp::Time last_transform_error_time_;

  //RCLCPP_INFO(get_logger(), "mainNodeThread 2");

  if(this->people_received ||true)
  {
    this->people_mutex_.enter(); 
    
    //people = this->people_msg;
    
    if(this->people_msg_queue.size()!=0)
    {
      people = this->people_msg_queue.front();
      this->people_msg_queue.pop();
      publish=true;
    }
    else
    {
      //ROS_INFO("no queue");
    }
    this->people_mutex_.exit();
  
    if(publish)
    {
      //if(this->config.filter && people.detection.size()!=0)
      if(people.detection.size()!=0)
      {
        if(this->map_received)
        {
          if(people.header.frame_id != this->map_frame)
          {
            try
            {
              //std::string source_frame = "odom";//people.header.frame_id; Temporal!! IMPORTANTE 2025
              std::string source_frame = people.header.frame_id; // TODO: crear tf para map! para que funcione este nodo!!!       
              std::string target_frame = this->map_frame;
              
              //std::cout << "people.header.frame_id"<< people.header.frame_id << std::endl;
              //std::cout << "target_frame"<< target_frame << std::endl;
              
              //ros::Time   target_time  = people.header.stamp;
              rclcpp::Time target_time = people.header.stamp;
              // Convert rclcpp::Time to tf2::TimePoint
              tf2::TimePoint tf2_target_time = tf2::TimePoint(std::chrono::nanoseconds(target_time.nanoseconds()));

              alg_->unlock();
              //bool tf_exists = tf_listener_.waitForTransform(target_frame,  source_frame, target_time, ros::Duration(0.5), ros::Duration(0.01));
              // Use tf2::Duration for the timeout
              tf2::Duration timeout = tf2::durationFromSec(0.5);
              bool tf_exists = tf_buffer_.canTransform(target_frame, source_frame, tf2_target_time, timeout);
              alg_->lock();
              if(tf_exists)
              {
                geometry_msgs::msg::TransformStamped transformStamped = tf_buffer_.lookupTransform(target_frame, source_frame, rclcpp::Time(target_time));
                geometry_msgs::msg::PoseStamped pose_out;
                geometry_msgs::msg::PoseStamped pose_in;
                for(unsigned int i=0; i<people.detection.size(); i++)
                {
                  pose_in.header             = people.header;
                  pose_in.pose.position      = people.detection[i].position;
                  pose_in.pose.orientation.w = 1;
                  //tf_listener_.transformPose( target_frame, pose_in, pose_out);
                  //people.detection[i].position = pose_out.pose.position;
                  // Transform the pose using the tf2 buffer
                  try {
                      pose_out = tf_buffer_.transform(pose_in, target_frame); // Correct transformation call
                      people.detection[i].position = pose_out.pose.position; // Update position
                  } catch (const tf2::TransformException &ex) {
                      RCLCPP_ERROR(this->get_logger(), "Transform error: %s", ex.what());
                  }
                }
                people.header.frame_id = target_frame;
                //continue...
              }
              else
              {
                //ROS_ERROR_DELAYED_THROTTLE(5,"LaserPeopleMapFilterAlgNode::No transform: %s-->%s", source_frame.c_str(), target_frame.c_str());
                //people.detection.clear();
                rclcpp::Time current_time = this->get_clock()->now();
                if ((current_time - last_transform_error_time_).seconds() > 5.0) // Log every 5 seconds
                {
                    RCLCPP_ERROR(this->get_logger(), "LaserPeopleMapFilterAlgNode::No transform: %s-->%s", source_frame.c_str(), target_frame.c_str());
                    last_transform_error_time_ = current_time; // Update last logged time
                }
                people.detection.clear();
                //continue...
              }
            }
            /*catch (tf::TransformException &ex)
            {
              ROS_ERROR("LaserPeopleMapFilterAlgNode:: %s",ex.what());
              people.detection.clear();
              //continue...
            }*/
            catch (const tf2::TransformException &ex)
            {
              RCLCPP_ERROR(this->get_logger(), "Transform error: %s", ex.what());
              people.detection.clear();
            }
          }
          else
          {
            //no transform need
            //continue...
          }
          
          //continue with map filter
          if(people.detection.size()!=0)
          {
            this->detectionArray_msg_ = iri_perception_msgs::msg::DetectionArray();
            this->detectionArray_msg_.header= people.header;
            for(unsigned int i=0; i<people.detection.size() ; i++)
            {
              float x = people.detection[i].position.x;
              float y = people.detection[i].position.y;
              float res = this->map.info.resolution;
              int width = this->map.info.width;
              //int height= this->map.info.height;
              float x0 = this->map.info.origin.position.x;
              float y0 = this->map.info.origin.position.y;
              //float yaw = tf2::getYaw(this->map.info.origin.orientation);
              geometry_msgs::msg::Quaternion q = this->map.info.origin.orientation;
              float yaw = tf2::getYaw(q); // Correct usage of tf2::getYaw
              // does yaw!=0 work?
              float xx = (x-x0)*cos(-yaw)-(y-y0)*sin(-yaw); //undo translation and rotation
              float yy = (x-x0)*sin(-yaw)+(y-y0)*cos(-yaw); //undo translation and rotation
              int grid_x = (unsigned int)(xx / res);
              int grid_y = (unsigned int)(yy / res);
              int index = grid_y*width + grid_x;
              int neighborDistance = int(this->neighborRadius/res);
              if(this->checkCellNearObstacle(index, neighborDistance))
              {
                this->detectionArray_msg_.detection.push_back(people.detection[i]);
              }
            }
          }
          
        }
        else
        {
          //ROS_ERROR_DELAYED_THROTTLE(5,"LaserPeopleMapFilterAlgNode::mainNodeThread: no map received, no filtering done!");
          RCLCPP_ERROR(this->get_logger(), "LaserPeopleMapFilterAlgNode::mainNodeThread: no map received, no filtering done!");
          //people.detection.clear();  // removed in 2025 to vaipas filter when there is no map. we can cahnge in futre if need.
          //this->detectionArray_msg_ = iri_perception_msgs::msg::DetectionArray(); // removed in 2025 to vaipas filter when there is no map. we can cahnge in futre if need.
           //no filter
          this->detectionArray_msg_ = people; // added in 2025 to vaipas filter when there is no map. we can cahnge in futre if need.
        }
      }
      else
      {
        //no filter
        this->detectionArray_msg_ = people;
      }

      this->getPeopleMarkers(this->detectionArray_msg_, this->MarkerArray_msg_);
      //this->peopleMarkers_publisher_.publish(this->MarkerArray_msg_);
      this->peopleMarkers_publisher_->publish(this->MarkerArray_msg_);
      //this->mapFilteredPeople_publisher_.publish(this->detectionArray_msg_);
      this->mapFilteredPeople_publisher_->publish(this->detectionArray_msg_);
      this->people_received=false;
    }
  }
  else
  {
    //no new people received
  }
  
  alg_->unlock();

  // [fill msg structures]
  //this->MarkerArray_msg_.data = my_var;
  //this->detectionArray_msg_.data = my_var;
  
  // [fill srv structure and make request to the server]
  
  // [fill action structure and make request to the action server]

  // [publish messages]
  //this->peopleMarkers_publisher_.publish(this->MarkerArray_msg_);
  //this->mapFilteredPeople_publisher_.publish(this->detectionArray_msg_);
    //RCLCPP_INFO(get_logger(), "mainNodeThread last");
}

/*  [subscriber callbacks] */
void LaserPeopleMapFilterAlgNode::map_callback(const nav_msgs::msg::OccupancyGrid::ConstPtr& msg) 
{ 
  RCLCPP_INFO(get_logger(), "map_callback in");
  //ROS_DEBUG("LaserPeopleMapFilterAlgNode::map_callback: New Message Received"); 

  //use appropiate mutex to shared variables if necessary 
  alg_->lock(); 
  this->map_mutex_.enter(); 
  if(!map_received){
    //ROS_INFO("LaserPeopleMapFilterAlgNode::map_callback: map Received");
    this->map = *msg; 
    this->map_received = true;
    this->map_frame = msg->header.frame_id;
    
    std::cout << "map_frame"<< map_frame << std::endl;
    std::cout << "msg->header.frame_id"<< msg->header.frame_id << std::endl;
    //this->map_frame = "odom"; //msg->header.frame_id; solucion temporal IMPORTANTE 2025
  }

  //unlock previously blocked shared variables 
  alg_->unlock(); 
  this->map_mutex_.exit(); 
}

void LaserPeopleMapFilterAlgNode::people_callback(const iri_perception_msgs::msg::DetectionArray::ConstPtr& msg) 
{ 
     //RCLCPP_INFO(get_logger(), "people_callback in");
  //ROS_DEBUG("LaserPeopleMapFilterAlgNode::people_callback: New Message Received"); 

  //use appropiate mutex to shared variables if necessary 
  this->alg_->lock(); 
  this->people_mutex_.enter(); 
  
  this->people_msg = *msg;
  this->people_msg_queue.push(*msg);
  if(this->people_msg_queue.size()>4) //TODO: need to be limited? limit somehow else?
    this->people_msg_queue.pop();
  //ROS_INFO("LaserPeopleMapFilterAlgNode::people_callback: people_msg_queue.size()=%lu",this->people_msg_queue.size());
  this->people_received=true;

  //unlock previously blocked shared variables 
  this->people_mutex_.exit(); 
  this->alg_->unlock(); 
  
}

/*  [service callbacks] */

/*  [action callbacks] */

/*  [action requests] */

/*void LaserPeopleMapFilterAlgNode::node_config_update(Config &config, uint32_t level)
{
  this->alg_->lock();
  this->config=config;
  this->neighborRadius    = config.neighborRadius;
  this->markerWidth       = config.markerWidth;
  this->markerHeight      = config.markerHeight;
  this->markerR           = config.markerR;
  this->markerG           = config.markerG;
  this->markerB           = config.markerB;
  this->markerA           = config.markerA;
  this->alg_->unlock();
}*/

void LaserPeopleMapFilterAlgNode::addNodeDiagnostics(void)
{
}

/* main function */
int main(int argc,char *argv[])
{

   rclcpp::init(argc, argv);
    auto node = std::make_shared<LaserPeopleMapFilterAlgNode>();
    //auto node = LaserPeopleDetectionAlgNode::create(); 
    //node.mainnodethread();
    //std::thread node_thread(mainnodethread(), node);
    //node_thread.join();
   //auto node = std::make_shared<LaserPeopleDetectionAlgNode>();
   //std::shared_ptr<LaserPeopleDetectionAlgNode> node = std::make_shared<LaserPeopleDetectionAlgNode>();
   
    //RCLCPP_INFO(node->get_logger(), "main 1");
    //rclcpp::Rate loop_rate(node->getRate());
    //RCLCPP_INFO(node->get_logger(), "main 2");
   
   while (rclcpp::ok())
  {
    //RCLCPP_INFO(node->get_logger(), "main 2");
    node->mainNodeThread();
    //RCLCPP_INFO(node->get_logger(), "main 3");
    rclcpp::spin(node);
    //rclcpp::spin_some(node);
    //RCLCPP_INFO(node->get_logger(), "main 4");
 
  }
  //RCLCPP_INFO(node->get_logger(), "main 7");
  rclcpp::shutdown();
   
   //rclcpp::spin(node); 
   //rclcpp::shutdown();
   
  //return algorithm_base::main<LaserPeopleDetectionAlgNode>(argc, argv, "laser_people_detection_alg_node");
  return 0;

  //return algorithm_base::main<LaserPeopleMapFilterAlgNode>(argc, argv, "laser_people_map_filter_alg_node");
}

bool LaserPeopleMapFilterAlgNode::checkCellNearObstacle(int index, int neighborDistance)
{
  
  //returns if the index cell is valid or not (obstacles near = not valid)
  bool valid = true;
  int      W = this->map.info.width;
  int      N = neighborDistance;

  if(index>0 && index < signed(this->map.data.size()))
  {
    if(this->map.data[index]!=0)
    {
      valid=false;
    }
    else
    {
      // go over the distance-N neighbors
      for(int j=-N; j<N; j++)
      {
        for(int i=-N; i<N; i++)
        {
          int indexi = i*W+j+index;
          // check if indexi is in the map.data size
          if(indexi >0 && indexi < signed(this->map.data.size()))
          {
            //if the indexi cell is not free (!=0) the main cell is not valid
            if(this->map.data[indexi]!=0)
            {
              valid=false;
              break;
            }
          }
          else
          {
            if(indexi==index)
            {
              valid=false;
              break;
            }
          }
        }
        if(!valid)
          break;
      }
    }
  }
  else
  {
    //ROS_ERROR("LaserPeopleMapFilterAlgNode: map index out of bounds");
    RCLCPP_ERROR(this->get_logger(), "LaserPeopleMapFilterAlgNode: map index out of bounds");
    valid=false;
  }

  return valid;
}

void LaserPeopleMapFilterAlgNode::getPeopleMarkers(iri_perception_msgs::msg::DetectionArray & peopleList, visualization_msgs::msg::MarkerArray & people)
{
  static unsigned int lastSize=0;
  people.markers.resize(std::max(uint(peopleList.detection.size()), lastSize));
  lastSize = peopleList.detection.size();

  for(unsigned int i=0; i<peopleList.detection.size(); i++)
  {
    people.markers[i].scale.x = this->markerWidth;
    people.markers[i].scale.y = this->markerWidth;
    people.markers[i].scale.z = this->markerHeight + peopleList.detection[i].probability;

    people.markers[i].pose.position.x = peopleList.detection[i].position.x;
    people.markers[i].pose.position.y = peopleList.detection[i].position.y;
    people.markers[i].pose.position.z = people.markers[i].scale.z/2.0 -0.44;

    people.markers[i].header.frame_id = peopleList.header.frame_id;
    people.markers[i].header.stamp    = peopleList.header.stamp;

    people.markers[i].id = i;
    people.markers[i].type = visualization_msgs::msg::Marker::CYLINDER;
    people.markers[i].action = visualization_msgs::msg::Marker::ADD;

    //people.markers[i].lifetime = ros::Duration(0.5f);
    people.markers[i].lifetime = rclcpp::Duration::from_seconds(0.5f);

    people.markers[i].pose.orientation.x = 0.0;
    people.markers[i].pose.orientation.y = 0.0;
    people.markers[i].pose.orientation.z = 0.0;
    people.markers[i].pose.orientation.w = 1.0;

    people.markers[i].color.r = this->markerR;
    people.markers[i].color.g = this->markerG;
    people.markers[i].color.b = this->markerB;
    people.markers[i].color.a = this->markerA;
  }

  //Delete extra previous markers;
  for(unsigned int i=peopleList.detection.size(); i<people.markers.size(); i++)
  {
    people.markers[i].action = visualization_msgs::msg::Marker::DELETE;
  }
}
