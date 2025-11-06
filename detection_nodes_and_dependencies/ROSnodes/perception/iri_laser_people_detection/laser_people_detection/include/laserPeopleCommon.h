#ifndef _LASERPEOPLECOMMON_H
#define _LASERPEOPLECOMMON_H

#include <iostream>
#include <vector>
#include <utility>   //std::pair<first,second>
#include <math.h>    //cos,sin,pow,sqrt,fabs,atan2
#include <algorithm> //min_element

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>


//using namespace std;
//using namespace Eigen;

#ifndef _CONST_H_
#define _CONST_H_
const unsigned int NUMBER_OF_FEATURES = 14;
#endif

/*!\class CLaserPeopleCommon
  \brief Common Class for data processing
  From a vector of points, creates segments and extracts their set of features.
  - Receives the data as a vector of ranges, angles minimum, maximum and resolution.
  - Converts the data from Polar to Cartesian coordinates
  - Makes clusters of points (segments) using a jump distance threshold
  - Computes a set of features to describe each segment.
*/
class CLaserPeopleCommon
{
  protected:
    float angle_min;        //!<start angle of the scan [rad]
    float angle_max;        //!<end angle of the scan [rad]
    float angle_increment;  //!<angular distance between measurements [rad]
    float jump_distance;    //!<threshold distance between consecutive points to start a new segment [m]
    unsigned int   min_points;       //!<minimum number of points for a segment to be considered

    std::vector< float > ranges; //!<vector of point ranges [m]

    /**
     * \brief Clears vector variables
     */
    void initialize();

    /**
     * \brief Converts from polar coordinates in ranges[] to cartesian coordinates in points[]
     */
    void extractPoints();

    /**
     * \brief Makes groups of consecutive points by jump distance, storing them in segments[] with its first and last point
     */
    void extractSegments();

    /**
     * \brief Calculates the features of each segment, storing them in features[]
     */
    void extractFeatures();

  public:
    CLaserPeopleCommon(); //!< constructor
    ~CLaserPeopleCommon(); //!< destructor

    bool debug; //!< show debug output

    std::vector< std::pair<double,double> > points;   //!<vector of pairs with Cartesian points x,y [m][m]
    std::vector< std::pair<int,int> >       segments; //!<vector of pairs with segments first/last points
    std::vector< std::vector<double> >     features; //!<vector of vectors of feature values of each segment

    /** \brief Using ranges and params performs the iteration sequence of data (points, segments, features)
     * \param ranges vector of points from the laser sensor
     * \param angle_min angle of the first point in the scan
     * \param angle_max angle of the last point in the scan
     * \param angle_increment resolution, angle between consecutive points in the scan
     */
    void scanIteration(const std::vector<float> ranges, const float angle_min, const float angle_max, const float angle_increment);

    /**
     * \brief Using ranges and params performs the iteration sequence of data, except for the features (points, segments)
     * \param ranges vector of points from the laser sensor
     * \param angle_min angle of the first point in the scan
     * \param angle_max angle of the last point in the scan
     * \param angle_increment resolution, angle between consecutive points in the scan
     */
    void scanIteration2(const std::vector<float> ranges, const float angle_min, const float angle_max, const float angle_increment);

    /**
     * \brief Sets jump distance for segmentation, in meters
     */
    void setJumpDistance(const float jumpDistance);

    /**
     * \brief Sets minimum number of points for segmentation
     */
    void setMinPoints(const unsigned int minPoints);

    /**
     * \brief Get standard deviation of segment, in meters
     */
    float getSegmentDeviation(const unsigned int segment);
};

#endif
