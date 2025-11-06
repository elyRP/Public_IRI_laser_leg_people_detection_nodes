#include "laser_people_detection_alg.h"

LaserPeopleDetectionAlgorithm::LaserPeopleDetectionAlgorithm(void)
{
  //RCLCPP_INFO(rclcpp::get_logger("NombrePersonalizado"), "LaserPeopleDetectionAlgorithm creo clase CLaserPeopleDetection");
  myLaserPeopleDetection = new CLaserPeopleDetection();
}

LaserPeopleDetectionAlgorithm::~LaserPeopleDetectionAlgorithm(void)
{
  delete myLaserPeopleDetection;
}

/*void LaserPeopleDetectionAlgorithm::config_update(Config& new_cfg, uint32_t level)
{
  this->lock();

  // save the current configuration
  this->setRangeThreshold(float(new_cfg.rangeThreshold));
  this->setDetectionThreshold(float(new_cfg.detectionThreshold));
  myLaserPeopleDetection->setPersonRadius(new_cfg.personRadius);
  myLaserPeopleDetection->setJumpDistance(new_cfg.jumpDistance);
  myLaserPeopleDetection->setMinPoints(new_cfg.minPoints);
  std::string newPath(new_cfg.boostFilePath);
  std::string newPath2(new_cfg.boostFilePath2);
  this->setBoostFilePaths(newPath, newPath2);
  this->config_=new_cfg;

  this->unlock();
}*/

// LaserPeopleDetectionAlgorithm Public API

void LaserPeopleDetectionAlgorithm::iteration(const sensor_msgs::msg::LaserScan scan, iri_perception_msgs::msg::DetectionArray & people)
{
    //RCLCPP_INFO(rclcpp::get_logger("NombrePersonalizado"), "LaserPeopleDetectionAlgorithm entro ok en iteration()");
  // Using scan ranges and params, performs a detection iteration, storing people results in peopleList[]
  std::vector<std::vector<float> > peopleList;
  
  if (myLaserPeopleDetection){
    //RCLCPP_INFO(rclcpp::get_logger("NombrePersonalizado"), "Detector OK!!!");
    myLaserPeopleDetection->detectionIteration(scan.ranges, scan.angle_min, scan.angle_max, scan.angle_increment, &peopleList);
  }else{
    RCLCPP_INFO(rclcpp::get_logger("NombrePersonalizado"), "Detector is null!");
  }


  // Fills people message with header and people data
  people.header.frame_id = scan.header.frame_id;
  people.header.stamp    = scan.header.stamp;
  people.type            = 0;  //laser detector type

  people.detection.resize(peopleList.size());
  for(unsigned int i=0; i<peopleList.size(); i++)
  {
    people.detection[i].position.x     = peopleList[i][0]; // x coordinate    of person [i]
    people.detection[i].position.y     = peopleList[i][1]; // y coordinate    of person [i]
    people.detection[i].position.z     = 0;
    people.detection[i].probability    = peopleList[i][2]; // p quality value of person [i]
    people.detection[i].covariances[0] = peopleList[i][3]; // covariance xx
    people.detection[i].covariances[7] = peopleList[i][3]; // covariance yy
    people.detection[i].id             = 0;
  }

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
