#include "laser_people_detection_alg_node.h"

LaserPeopleDetectionAlgNode::LaserPeopleDetectionAlgNode(void) : 
  Node("laser_people_detection_node"),
  //algorithm_base::IriBaseAlgorithm<LaserPeopleDetectionAlgorithm>(),
  new_laser_event_id_("new_laser_event"),
  //tf_buffer_(std::make_shared<rclcpp::Clock>(RCL_ROS_TIME)),
  //tf_listener_(tf_buffer_, rclcpp::Duration::from_seconds(10.0)),
  tf_buffer_(this->get_clock()),
  target_frame("/base_link")
  //rate_(0.2)
{
//RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo step1 ");
  //init class attributes if necessary
 // this->setRate(50);//in [Hz]
 

 alg_ = std::make_shared<LaserPeopleDetectionAlgorithm>();
 
// RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 2 ");
  using std::placeholders::_1;  // se puede poner dentro del constructor o arriba del todo
 // timer_ = this->create_wall_timer(
  //20ms, std::bind(&LaserPeopleDetectionAlgNode::mainNodeThread, this->rclcpp::Node::shared_from_this()));
// RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 3 ");
  auto callback = std::bind(&LaserPeopleDetectionAlgNode::mainNodeThread, this);
  //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 4 ");
  timer_ = this->create_wall_timer(20ms, callback);
   //RCLCPP_INFO(this->get_logger(), "Node constructed");
   //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 5 ");
  this->init();
//RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 6 ");
  // [init publishers]
  //this->peopleMarkers_array_publisher_ = this->public_node_handle_.advertise<visualization_msgs::MarkerArray>("markers", 1);
  this->peopleMarkers_array_publisher_ = this->create_publisher<visualization_msgs::msg::MarkerArray>("/markers", 1);
  //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 7 ");
  //this->people_publisher_ = this->public_node_handle_.advertise<iri_perception_msgs::detectionArray>("people", 1);
  this->people_publisher_ = this->create_publisher<iri_perception_msgs::msg::DetectionArray>("/people", 1);
//RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo 8 ");
  // [init subscribers]
  //this->scan_subscriber_ = this->public_node_handle_.subscribe("scan", 1, &LaserPeopleDetectionAlgNode::scan_callback, this);
  this->scan_subscriber_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
    "/scan", 1, std::bind(&LaserPeopleDetectionAlgNode::scan_callback, this, std::placeholders::_1));

  // [init services]

  // [init clients]

  // [init action servers]

  // [init action clients]
RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo last-step ");
}

std::shared_ptr<LaserPeopleDetectionAlgNode> LaserPeopleDetectionAlgNode::create()
{
    auto node = std::shared_ptr<LaserPeopleDetectionAlgNode>(new LaserPeopleDetectionAlgNode());
    //node->init();  // ahora sí es seguro usar shared_from_this()
    return node;
}


LaserPeopleDetectionAlgNode::~LaserPeopleDetectionAlgNode(void)
{
  // [free dynamic memory]
}

void LaserPeopleDetectionAlgNode::init()
{
 //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo INIT 1 ");
  this->selectPosesFrame=false;
  this->selectScanFrame=false;
  this->scanFrame="/front_laser";
  this->markerWidth=0.5;
  this->markerHeight=0.2;
  this->markerR=1.0;
  this->markerG=0.0;
  this->markerB=0.0;
  this->markerA=0.75;

  this->filterPosesMode=false;
  this->filterR=0.0;
  this->filterXmin=0.0;
  this->filterXmax=0.0;
  this->filterYmin=0.0;
  this->filterYmax=0.0;
  
  event_server_ = CEventServer::instance();
  event_server_->create_event(new_laser_event_id_);

  //public_node_handle_.getParam("posesFrame", this->target_frame);
  //public_node_handle_.getParam("scanFrame", this->scanFrame);
   //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo INIT 2 ");
  this->declare_parameter("posesFrame", "/base_link");  // Default value
  this->declare_parameter("scanFrame", "/scan");         // Default value
  this->declare_parameter("rangeThreshold", 0.0);
  this->declare_parameter("detectionThreshold", -0.05);
  this->declare_parameter("boostFilePath", "/usr/local/include/iri/laser_people_detection/boostData/boostFile.txt");
  this->declare_parameter("boostFilePath2", "/usr/local/include/iri/laser_people_detection/boostData/boostFile2.txt");
   //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo INIT 3 ");
  this->declare_parameter("selectPosesFrame",  false);
   //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo INIT 4 ");
  //this->declare_parameter("selectScanFrame", true);  
  this->declare_parameter("selectScanFrame", false);
   //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo INIT 5 ");
  this->declare_parameter("markerWidth", 0.7);
  this->declare_parameter("markerHeight", 0.5);
  this->declare_parameter("markerR", 1.0);
  this->declare_parameter("markerG", 0.0);
  this->declare_parameter("markerB", 0.0);
  this->declare_parameter("markerA", 0.5);
  this->declare_parameter("filterPosesMode", false);
  this->declare_parameter("filterR", 0.0);
  this->declare_parameter("filterXmin", 0.0);
  this->declare_parameter("filterXmax", 0.0);
  this->declare_parameter("filterYmin", 0.0);
  this->declare_parameter("filterYmax", 0.0);
  this->declare_parameter("personRadius", 0.7);
  this->declare_parameter("jumpDistance", 0.1);
  this->declare_parameter("minPoints", 3);
    
        
 // this->get_parameter("posesFrame", this->target_frame);
 // this->get_parameter("scanFrame", this->scanFrame);
  //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo INIT 7 ");
/*tf_listener_ = std::make_shared<tf2_ros::TransformListener>(
    tf_buffer_,
    this->LaserPeopleDetectionAlgNode::shared_from_this()
   );*/
   //RCLCPP_INFO(rclcpp::Node::get_logger(), "laser-people-detection: Inicio creador nodo INIT last ");
}

/*void LaserPeopleDetectionAlgNode::mainNodeThread()
{
  // Verificar si hay nuevo evento de láser
  if (event_server_->event_is_set(new_laser_event_id_))
  {
    event_server_->reset_event(new_laser_event_id_);

    std::scoped_lock<std::mutex> lock(scan_mutex_);  // en ROS 2, usa scoped_lock para seguridad

    // Limpia el mensaje de detecciones
    detectionArray_msg_.detection.clear();

    // Ejecuta una iteración de detección
    alg_.iteration(currentScan, detectionArray_msg_);

    // Obtener y publicar los marcadores
    getPeopleMarkers(detectionArray_msg_, MarkerArray_msg_);
    peopleMarkers_array_publisher_->publish(MarkerArray_msg_);

    // Transformar poses si se solicita
    if (selectPosesFrame)
      transformPoses(detectionArray_msg_);

    // Filtrar poses si se solicita
    if (filterPosesMode)
      filterPoses(detectionArray_msg_);

    // Publicar detecciones de personas
    people_publisher_->publish(detectionArray_msg_);
  }

  // TODO: Lógica para:
  // - completar estructuras de mensajes
  // - llamadas a servicios
  // - llamadas a servidores de acciones
  // - publicaciones adicionales
}*/


void LaserPeopleDetectionAlgNode::mainNodeThread(void)
{

 //RCLCPP_INFO(get_logger(), "IN mainNodeThread function 1");
 
  if(event_server_->event_is_set(new_laser_event_id_))
  {
     //RCLCPP_INFO(get_logger(), "IN mainNodeThread function 2");
    event_server_->reset_event(new_laser_event_id_);
     //RCLCPP_INFO(get_logger(), "IN mainNodeThread function 3");
    scan_mutex_.enter();
     //RCLCPP_INFO(get_logger(), "IN mainNodeThread function 4");
    //Detection iteration
    this->detectionArray_msg_.detection.clear();
     //RCLCPP_INFO(get_logger(), "IN mainNodeThread function 5");

    
    alg_->iteration(this->currentScan, this->detectionArray_msg_);
    //this->iteration(this->currentScan, this->detectionArray_msg_);
     //RCLCPP_INFO(get_logger(), "IN mainNodeThread function 6");
    scan_mutex_.exit();
    
// RCLCPP_INFO(get_logger(), "IN mainNodeThread function 7");
    //Get and Publish Markers
    this->getPeopleMarkers(this->detectionArray_msg_, this->MarkerArray_msg_);
    this->peopleMarkers_array_publisher_->publish(this->MarkerArray_msg_);
// RCLCPP_INFO(get_logger(), "IN mainNodeThread function 8");
 
    //Transform people to desired frame
    if(this->selectPosesFrame)
      this->transformPoses(this->detectionArray_msg_);
    
    // RCLCPP_INFO(get_logger(), "IN mainNodeThread function 9");
    //Filter people (by radius or xy)
    if(this->filterPosesMode)
      this->filterPoses(this->detectionArray_msg_);
    
   //  RCLCPP_INFO(get_logger(), "IN mainNodeThread function 10");
    //Publish people
    this->people_publisher_->publish(this->detectionArray_msg_);
    // RCLCPP_INFO(get_logger(), "IN mainNodeThread function 11");
  }

  // [fill msg structures]

  // [fill srv structure and make request to the server]

  // [fill action structure and make request to the action server]

  // [publish messages]

}

/*  [subscriber callbacks] */
void LaserPeopleDetectionAlgNode::scan_callback(const sensor_msgs::msg::LaserScan::ConstPtr& msg)
{
  //ROS_INFO("LaserPeopleDetectionAlgNode::scan_callback: New Message Received");

  //use appropiate mutex to shared variables if necessary
  //this->alg_.lock();
  this->scan_mutex_.enter();
  if(this->selectScanFrame && msg->header.frame_id==this->scanFrame)
    this->currentScan = *msg;

  if(!this->selectScanFrame)
    this->currentScan = *msg;
  //unlock previously blocked shared variables
  //this->alg_.unlock();
  this->scan_mutex_.exit();

  if( !event_server_->event_is_set(new_laser_event_id_) )
    event_server_->set_event(new_laser_event_id_);
}

/*  [service callbacks] */

/*  [action callbacks] */

/*  [action requests] */

/*  [class functions] */
void LaserPeopleDetectionAlgNode::transformPoses(iri_perception_msgs::msg::DetectionArray & peoplePoses)
{
  iri_perception_msgs::msg::DetectionArray transformedPeoplePoses(peoplePoses);
  transformedPeoplePoses.header          = peoplePoses.header;
  std::string source_frame               = transformedPeoplePoses.header.frame_id;
  std::string target_frame               = this->target_frame;
  rclcpp::Time   source_time             = transformedPeoplePoses.header.stamp;
  transformedPeoplePoses.header.frame_id = this->target_frame;

  // Get the current time
  rclcpp::Time target_time = this->now();

  /*try
  {
    rclcpp::Time target_time = transformedPeoplePoses.header.stamp;
    bool tf_exists = tf_listener_.waitForTransform(target_frame, source_frame, target_time, ros::Duration(5), ros::Duration(0.01));
    if(tf_exists)
    {
      for(unsigned int i=0; i<transformedPeoplePoses.detection.size(); i++)
      {
        geometry_msgs::PoseStamped poseIn;
        geometry_msgs::PoseStamped poseOut;
        poseIn.header             = transformedPeoplePoses.header;
        poseIn.header.frame_id    = source_frame;
        poseIn.pose.position      = transformedPeoplePoses.detection[i].position;
        poseIn.pose.orientation.z = 1.0;
        tf_listener_.transformPose(target_frame,     poseIn,     poseOut);
        transformedPeoplePoses.detection[i].position = poseOut.pose.position;
        transformedPeoplePoses.detection[i].probability   = transformedPeoplePoses.detection[i].probability;
      }
    }
    else
    {
      ROS_ERROR("LaserPeopleDetectionAlgNode::No transform: %s-->%s", source_frame.c_str(), target_frame.c_str());
    }
  }
  catch (tf::TransformException &ex)
  {
    ROS_ERROR("LaserPeopleDetectionAlgNode:: %s",ex.what());
  }
  peoplePoses = transformedPeoplePoses;
}*/
  try
  {
    // Get the timestamp from the detection array
    rclcpp::Time target_time = transformedPeoplePoses.header.stamp;
    std::string source_frame = transformedPeoplePoses.header.frame_id;

    // Check if the transform is available
    if (tf_buffer_.canTransform(target_frame, source_frame, target_time, tf2::Duration(5)))
    {
       // rplidar_link
       //std::cout<<"rplidar_link"<<std::endl;
      // Look up the transformation
      geometry_msgs::msg::TransformStamped transform_stamped;
      transform_stamped = tf_buffer_.lookupTransform(target_frame, source_frame, target_time);

      // Loop through all detections and apply the transform
      for (unsigned int i = 0; i < transformedPeoplePoses.detection.size(); i++)
      {
        geometry_msgs::msg::PoseStamped poseIn;
        geometry_msgs::msg::PoseStamped poseOut;

        poseIn.header = transformedPeoplePoses.header;
        poseIn.header.frame_id = source_frame;
        poseIn.pose.position = transformedPeoplePoses.detection[i].position;
        poseIn.pose.orientation.z = 1.0;

        // Apply the transformation
        tf2::doTransform(poseIn, poseOut, transform_stamped);

        // Update the transformed pose in the detection array
        transformedPeoplePoses.detection[i].position = poseOut.pose.position;
        transformedPeoplePoses.detection[i].probability = transformedPeoplePoses.detection[i].probability;
      }
    }
    else
    {
        RCLCPP_ERROR(rclcpp::Node::get_logger(), "No transform: %s --> %s", source_frame.c_str(), target_frame.c_str());
    }
  }
  catch (tf2::TransformException &ex)
  {
    RCLCPP_ERROR(rclcpp::Node::get_logger(), "%s", ex.what());
  }

  // Assign the transformed people poses to the output
  peoplePoses = transformedPeoplePoses;

}

void LaserPeopleDetectionAlgNode::filterPoses(iri_perception_msgs::msg::DetectionArray & peoplePoses)
{
  iri_perception_msgs::msg::DetectionArray filteredPoses;
  filteredPoses.header = peoplePoses.header;

  for(unsigned int i=0; i<peoplePoses.detection.size(); i++)
  {
    double x=peoplePoses.detection[i].position.x;
    double y=peoplePoses.detection[i].position.y;
    bool goodPose=true;
    if( (this->filterXmin!=0 || this->filterXmax!=0) && (x<this->filterXmin || x>this->filterXmax) )
      goodPose=false;
    if( (this->filterYmin!=0 || this->filterYmax!=0) && (y<this->filterYmin || y>this->filterYmax) )
      goodPose=false;
    if( (this->filterR!=0) && (x*x+y*y > this->filterR*this->filterR ))
      goodPose=false;
    if(goodPose)
      filteredPoses.detection.push_back(peoplePoses.detection[i]);
  }
  peoplePoses = filteredPoses;
}

void LaserPeopleDetectionAlgNode::getPeopleMarkers(iri_perception_msgs::msg::DetectionArray & peopleList, visualization_msgs::msg::MarkerArray & people)
{
  static unsigned int lastSize=0;
  people.markers.resize(std::max(uint(peopleList.detection.size()), lastSize));
  lastSize = peopleList.detection.size();

  for(unsigned int i=0; i<peopleList.detection.size(); i++)
  {
    people.markers[i].header.frame_id = peopleList.header.frame_id;
    people.markers[i].header.stamp    = this->currentScan.header.stamp;
    people.markers[i].ns              = "people";
    people.markers[i].id              = i;
    people.markers[i].type            = visualization_msgs::msg::Marker::CYLINDER;
    people.markers[i].action          = visualization_msgs::msg::Marker::ADD;

    people.markers[i].scale.x = this->markerWidth;
    people.markers[i].scale.y = this->markerWidth;
    people.markers[i].scale.z = this->markerHeight + peopleList.detection[i].probability;

    people.markers[i].pose.position.x = peopleList.detection[i].position.x;
    people.markers[i].pose.position.y = peopleList.detection[i].position.y;
    people.markers[i].pose.position.z = people.markers[i].scale.z/2.0 -0.44;

    people.markers[i].pose.orientation.x = 0.0;
    people.markers[i].pose.orientation.y = 0.0;
    people.markers[i].pose.orientation.z = 0.0;
    people.markers[i].pose.orientation.w = 1.0;

    people.markers[i].color.r = this->markerR;
    people.markers[i].color.g = this->markerG;
    people.markers[i].color.b = this->markerB;
    people.markers[i].color.a = this->markerA;
    people.markers[i].lifetime = rclcpp::Duration::from_seconds(0.5);
  }

  //Delete extra previous markers;
  for(unsigned int i=peopleList.detection.size(); i<people.markers.size(); i++)
  {
    people.markers[i].action = visualization_msgs::msg::Marker::DELETE;
  }

}

/*void LaserPeopleDetectionAlgNode::node_config_update(Config &config, uint32_t level)
{
  this->alg_.lock();
  this->selectPosesFrame  = config.selectPosesFrame;
  this->target_frame      = config.posesFrame;
  this->selectScanFrame   = config.selectScanFrame;
  this->scanFrame         = config.scanFrame;
  this->markerWidth       = config.markerWidth;
  this->markerHeight      = config.markerHeight;
  this->markerR           = config.markerR;
  this->markerG           = config.markerG;
  this->markerB           = config.markerB;
  this->markerA           = config.markerA;
  this->filterPosesMode   = config.filterPosesMode;
  this->filterR           = config.filterR;
  this->filterXmin        = config.filterXmin;
  this->filterXmax        = config.filterXmax;
  this->filterYmin        = config.filterYmin;
  this->filterYmax        = config.filterYmax;
  this->alg_.config_      = config;

  this->alg_.unlock();
}*/

void LaserPeopleDetectionAlgNode::addNodeDiagnostics(void)
{
}

/*
void LaserPeopleDetectionAlgorithm::iteration(const sensor_msgs::msg::LaserScan scan, iri_perception_msgs::msg::DetectionArray & people)
{

 RCLCPP_INFO(get_logger(), "IN iteration function 1");
  // Using scan ranges and params, performs a detection iteration, storing people results in peopleList[]
  std::vector<std::vector<float> > peopleList;
  RCLCPP_INFO(get_logger(), "IN iteration function 1.1");
  if (myLaserPeopleDetection)
    myLaserPeopleDetection->detectionIteration(scan.ranges, scan.angle_min, scan.angle_max, scan.angle_increment, &peopleList);
  else
    RCLCPP_ERROR(get_logger(), "Detector is null!");


 RCLCPP_INFO(get_logger(), "IN iteration function 2");
  // Fills people message with header and people data
  people.header.frame_id = scan.header.frame_id;
  people.header.stamp    = scan.header.stamp;
  people.type            = 0;  //laser detector type


 RCLCPP_INFO(get_logger(), "IN iteration function 3");
  people.detection.resize(peopleList.size());
  
   RCLCPP_INFO(get_logger(), "IN iteration function 4");
   
  for(unsigned int i=0; i<peopleList.size(); i++)
  { 
    RCLCPP_INFO(get_logger(), "IN iteration function 5");
    people.detection[i].position.x     = peopleList[i][0]; // x coordinate    of person [i]
    people.detection[i].position.y     = peopleList[i][1]; // y coordinate    of person [i]
    people.detection[i].position.z     = 0;
    people.detection[i].probability    = peopleList[i][2]; // p quality value of person [i]
    people.detection[i].covariances[0] = peopleList[i][3]; // covariance xx
    people.detection[i].covariances[7] = peopleList[i][3]; // covariance yy
    people.detection[i].id             = 0;
    
     RCLCPP_INFO(get_logger(), "IN iteration function 6");
  }
 RCLCPP_INFO(get_logger(), "IN iteration function 7");
}


void LaserPeopleDetectionAlgorithm::setRangeThreshold(const float rangeThreshold)
{
  myLaserPeopleDetection->setRangeThreshold(rangeThreshold);
}

void LaserPeopleDetectionAlgorithm::setDetectionThreshold(const float detectionThreshold)
{
  myLaserPeopleDetection->setDetectionThreshold(detectionThreshold);
}

void LaserPeopleDetectionAlgorithm::setBoostFilePaths(const std::string newPath, const std::string newPath2)
{
  myLaserPeopleDetection->setBoostFilePaths(newPath, newPath2);
}

 LaserPeopleDetectionAlgorithm::LaserPeopleDetectionAlgorithm(void)
{
  myLaserPeopleDetection = new CLaserPeopleDetection();
}

LaserPeopleDetectionAlgorithm::~LaserPeopleDetectionAlgorithm(void)
{
  delete myLaserPeopleDetection;
}*/

/* main function */
int main(int argc,char *argv[])
{

   rclcpp::init(argc, argv);
    auto node = std::make_shared<LaserPeopleDetectionAlgNode>();
    //auto node = LaserPeopleDetectionAlgNode::create(); 
    //node.mainnodethread();
    //std::thread node_thread(mainnodethread(), node);
    //node_thread.join();
   //auto node = std::make_shared<LaserPeopleDetectionAlgNode>();
   //std::shared_ptr<LaserPeopleDetectionAlgNode> node = std::make_shared<LaserPeopleDetectionAlgNode>();
   
    RCLCPP_INFO(node->get_logger(), "main 1");
    //rclcpp::Rate loop_rate(node->getRate());
    //RCLCPP_INFO(node->get_logger(), "main 2");
   
   while (rclcpp::ok())
  {
    //RCLCPP_INFO(node->get_logger(), "main 2");
    node->mainNodeThread();
    rclcpp::spin_some(node); // NO deja que los callback se activen desde minuto 0, por tanto necesitas un event_ cada vez que recives un callback.
      //RCLCPP_INFO(node->get_logger(), "main 3");
 
  }
  RCLCPP_INFO(node->get_logger(), "main 7");
  rclcpp::shutdown();
   
   //rclcpp::spin(node); 
   //rclcpp::shutdown();
   
  //return algorithm_base::main<LaserPeopleDetectionAlgNode>(argc, argv, "laser_people_detection_alg_node");
  return 0;
}
