#include "laser_people_detection_alg_node.h"

LaserPeopleDetectionAlgNode::LaserPeopleDetectionAlgNode(void) :
  algorithm_base::IriBaseAlgorithm<LaserPeopleDetectionAlgorithm>(),
  new_laser_event_id_("new_laser_event"),
  tf_listener_(ros::Duration(10.f)),
  target_frame("/base_link")
{
  //init class attributes if necessary
  this->setRate(50);//in [Hz]
  this->init();

  // [init publishers]
  this->peopleMarkers_array_publisher_ = this->public_node_handle_.advertise<visualization_msgs::MarkerArray>("markers", 1);
  this->people_publisher_ = this->public_node_handle_.advertise<iri_perception_msgs::detectionArray>("people", 1);

  // [init subscribers]
  this->scan_subscriber_ = this->public_node_handle_.subscribe("scan", 1, &LaserPeopleDetectionAlgNode::scan_callback, this);

  // [init services]

  // [init clients]

  // [init action servers]

  // [init action clients]

}

LaserPeopleDetectionAlgNode::~LaserPeopleDetectionAlgNode(void)
{
  // [free dynamic memory]
}

void LaserPeopleDetectionAlgNode::init()
{
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

  public_node_handle_.getParam("posesFrame", this->target_frame);
  public_node_handle_.getParam("scanFrame", this->scanFrame);
}

void LaserPeopleDetectionAlgNode::mainNodeThread(void)
{
  if(event_server_->event_is_set(new_laser_event_id_))
  {
    event_server_->reset_event(new_laser_event_id_);
    scan_mutex_.enter();
    //Detection iteration
    this->detectionArray_msg_.detection.clear();
    this->alg_.iteration(this->currentScan, this->detectionArray_msg_);
    scan_mutex_.exit();

    //Get and Publish Markers
    this->getPeopleMarkers(this->detectionArray_msg_, this->MarkerArray_msg_);
    this->peopleMarkers_array_publisher_.publish(this->MarkerArray_msg_);

    //Transform people to desired frame
    if(this->selectPosesFrame)
      this->transformPoses(this->detectionArray_msg_);
    
    //Filter people (by radius or xy)
    if(this->filterPosesMode)
      this->filterPoses(this->detectionArray_msg_);
    
    //Publish people
    this->people_publisher_.publish(this->detectionArray_msg_);
  }

  // [fill msg structures]

  // [fill srv structure and make request to the server]

  // [fill action structure and make request to the action server]

  // [publish messages]

}

/*  [subscriber callbacks] */
void LaserPeopleDetectionAlgNode::scan_callback(const sensor_msgs::LaserScan::ConstPtr& msg)
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
void LaserPeopleDetectionAlgNode::transformPoses(iri_perception_msgs::detectionArray & peoplePoses)
{
  iri_perception_msgs::detectionArray transformedPeoplePoses(peoplePoses);
  transformedPeoplePoses.header          = peoplePoses.header;
  std::string source_frame               = transformedPeoplePoses.header.frame_id;
  std::string target_frame               = this->target_frame;
  ros::Time   source_time                = transformedPeoplePoses.header.stamp;
  transformedPeoplePoses.header.frame_id = this->target_frame;

  try
  {
    ros::Time target_time = transformedPeoplePoses.header.stamp;
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
}

void LaserPeopleDetectionAlgNode::filterPoses(iri_perception_msgs::detectionArray & peoplePoses)
{
  iri_perception_msgs::detectionArray filteredPoses;
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

void LaserPeopleDetectionAlgNode::getPeopleMarkers(iri_perception_msgs::detectionArray & peopleList, visualization_msgs::MarkerArray & people)
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
    people.markers[i].type            = visualization_msgs::Marker::CYLINDER;
    people.markers[i].action          = visualization_msgs::Marker::ADD;

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
    people.markers[i].lifetime = ros::Duration(0.5f);
  }

  //Delete extra previous markers;
  for(unsigned int i=peopleList.detection.size(); i<people.markers.size(); i++)
  {
    people.markers[i].action = visualization_msgs::Marker::DELETE;
  }

}

void LaserPeopleDetectionAlgNode::node_config_update(Config &config, uint32_t level)
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
}

void LaserPeopleDetectionAlgNode::addNodeDiagnostics(void)
{
}

/* main function */
int main(int argc,char *argv[])
{
  return algorithm_base::main<LaserPeopleDetectionAlgNode>(argc, argv, "laser_people_detection_alg_node");
}
