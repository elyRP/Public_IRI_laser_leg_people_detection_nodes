#include "laser_people_map_filter_alg.h"

LaserPeopleMapFilterAlgorithm::LaserPeopleMapFilterAlgorithm(void)
{
}

LaserPeopleMapFilterAlgorithm::~LaserPeopleMapFilterAlgorithm(void)
{
}

void LaserPeopleMapFilterAlgorithm::config_update(Config& new_cfg, uint32_t level)
{
  this->lock();

  // save the current configuration
  this->config_=new_cfg;
  
  this->unlock();
}

// LaserPeopleMapFilterAlgorithm Public API