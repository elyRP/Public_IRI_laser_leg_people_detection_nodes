#include "laser_people_map_filter_alg_node.h"

LaserPeopleMapFilterAlgNode::LaserPeopleMapFilterAlgNode(void) :
  algorithm_base::IriBaseAlgorithm<LaserPeopleMapFilterAlgorithm>()
{
  //init class attributes if necessary
  this->setRate(100);//in [Hz]

  this->map_received=false;
  this->people_received=false;
  this->neighborRadius = 0.25; //m

  this->markerWidth=0.5;
  this->markerHeight=0.2;
  this->markerR=1.0;
  this->markerG=1.0;
  this->markerB=0.0;
  this->markerA=0.75;

  this->map_frame="noframe";

  this->public_node_handle_.getParam("neighborRadius", this->neighborRadius);

  // [init publishers]
  this->peopleMarkers_publisher_ = this->public_node_handle_.advertise<visualization_msgs::MarkerArray>("markers", 1);
  this->mapFilteredPeople_publisher_ = this->public_node_handle_.advertise<iri_perception_msgs::detectionArray>("people_out", 1);
  
  // [init subscribers]
  this->map_subscriber_ = this->public_node_handle_.subscribe("map", 1, &LaserPeopleMapFilterAlgNode::map_callback, this);
  this->people_subscriber_ = this->public_node_handle_.subscribe("people", 1, &LaserPeopleMapFilterAlgNode::people_callback, this);
  
  // [init services]
  
  // [init clients]
  
  // [init action servers]
  
  // [init action clients]
}

LaserPeopleMapFilterAlgNode::~LaserPeopleMapFilterAlgNode(void)
{
  // [free dynamic memory]
}

void LaserPeopleMapFilterAlgNode::mainNodeThread(void)
{
  this->alg_.lock();
  bool publish=false;
  
  iri_perception_msgs::detectionArray people;


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
      if(this->config.filter && people.detection.size()!=0)
      {
        if(this->map_received)
        {
          if(people.header.frame_id != this->map_frame)
          {
            try
            {
              std::string source_frame = people.header.frame_id;
              std::string target_frame = this->map_frame;
              ros::Time   target_time  = people.header.stamp;
              this->alg_.unlock();
              bool tf_exists = tf_listener_.waitForTransform(target_frame,  source_frame, target_time, ros::Duration(0.5), ros::Duration(0.01));
              this->alg_.lock();
              if(tf_exists)
              {
                geometry_msgs::PoseStamped pose_out;
                geometry_msgs::PoseStamped pose_in;
                for(unsigned int i=0; i<people.detection.size(); i++)
                {
                  pose_in.header             = people.header;
                  pose_in.pose.position      = people.detection[i].position;
                  pose_in.pose.orientation.w = 1;
                  tf_listener_.transformPose( target_frame, pose_in, pose_out);
                  people.detection[i].position = pose_out.pose.position;
                }
                people.header.frame_id = target_frame;
                //continue...
              }
              else
              {
                ROS_ERROR_DELAYED_THROTTLE(5,"LaserPeopleMapFilterAlgNode::No transform: %s-->%s", source_frame.c_str(), target_frame.c_str());
                people.detection.clear();
                //continue...
              }
            }
            catch (tf::TransformException &ex)
            {
              ROS_ERROR("LaserPeopleMapFilterAlgNode:: %s",ex.what());
              people.detection.clear();
              //continue...
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
            this->detectionArray_msg_ = iri_perception_msgs::detectionArray();
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
              float yaw = tf::getYaw(this->map.info.origin.orientation);
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
          ROS_ERROR_DELAYED_THROTTLE(5,"LaserPeopleMapFilterAlgNode::mainNodeThread: no map received, no filtering done!");
          people.detection.clear();
          this->detectionArray_msg_ = iri_perception_msgs::detectionArray();
        }
      }
      else
      {
        //no filter
        this->detectionArray_msg_ = people;
      }

      this->getPeopleMarkers(this->detectionArray_msg_, this->MarkerArray_msg_);
      this->peopleMarkers_publisher_.publish(this->MarkerArray_msg_);
      this->mapFilteredPeople_publisher_.publish(this->detectionArray_msg_);
      this->people_received=false;
    }
  }
  else
  {
    //no new people received
  }
  
  this->alg_.unlock();

  // [fill msg structures]
  //this->MarkerArray_msg_.data = my_var;
  //this->detectionArray_msg_.data = my_var;
  
  // [fill srv structure and make request to the server]
  
  // [fill action structure and make request to the action server]

  // [publish messages]
  //this->peopleMarkers_publisher_.publish(this->MarkerArray_msg_);
  //this->mapFilteredPeople_publisher_.publish(this->detectionArray_msg_);
}

/*  [subscriber callbacks] */
void LaserPeopleMapFilterAlgNode::map_callback(const nav_msgs::OccupancyGrid::ConstPtr& msg) 
{ 
  //ROS_DEBUG("LaserPeopleMapFilterAlgNode::map_callback: New Message Received"); 

  //use appropiate mutex to shared variables if necessary 
  this->alg_.lock(); 
  //this->map_mutex_.enter(); 
  if(!map_received){
    //ROS_INFO("LaserPeopleMapFilterAlgNode::map_callback: map Received");
    this->map = *msg; 
    this->map_received = true;
    this->map_frame = msg->header.frame_id;
  }

  //unlock previously blocked shared variables 
  this->alg_.unlock(); 
  //this->map_mutex_.exit(); 
}

void LaserPeopleMapFilterAlgNode::people_callback(const iri_perception_msgs::detectionArray::ConstPtr& msg) 
{ 
  //ROS_DEBUG("LaserPeopleMapFilterAlgNode::people_callback: New Message Received"); 

  //use appropiate mutex to shared variables if necessary 
  //this->alg_.lock(); 
  this->people_mutex_.enter(); 
  
  this->people_msg = *msg;
  this->people_msg_queue.push(*msg);
  if(this->people_msg_queue.size()>4) //TODO: need to be limited? limit somehow else?
    this->people_msg_queue.pop();
  //ROS_INFO("LaserPeopleMapFilterAlgNode::people_callback: people_msg_queue.size()=%lu",this->people_msg_queue.size());
  this->people_received=true;

  //unlock previously blocked shared variables 
  this->people_mutex_.exit(); 
  //this->alg_.unlock(); 
  
}

/*  [service callbacks] */

/*  [action callbacks] */

/*  [action requests] */

void LaserPeopleMapFilterAlgNode::node_config_update(Config &config, uint32_t level)
{
  this->alg_.lock();
  this->config=config;
  this->neighborRadius    = config.neighborRadius;
  this->markerWidth       = config.markerWidth;
  this->markerHeight      = config.markerHeight;
  this->markerR           = config.markerR;
  this->markerG           = config.markerG;
  this->markerB           = config.markerB;
  this->markerA           = config.markerA;
  this->alg_.unlock();
}

void LaserPeopleMapFilterAlgNode::addNodeDiagnostics(void)
{
}

/* main function */
int main(int argc,char *argv[])
{
  return algorithm_base::main<LaserPeopleMapFilterAlgNode>(argc, argv, "laser_people_map_filter_alg_node");
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
    ROS_ERROR("LaserPeopleMapFilterAlgNode: map index out of bounds");
    valid=false;
  }

  return valid;
}

void LaserPeopleMapFilterAlgNode::getPeopleMarkers(iri_perception_msgs::detectionArray & peopleList, visualization_msgs::MarkerArray & people)
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
    people.markers[i].type = visualization_msgs::Marker::CYLINDER;
    people.markers[i].action = visualization_msgs::Marker::ADD;

    people.markers[i].lifetime = ros::Duration(0.5f);

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
    people.markers[i].action = visualization_msgs::Marker::DELETE;
  }
}
