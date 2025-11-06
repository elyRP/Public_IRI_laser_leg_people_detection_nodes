#ifndef _LASERPEOPLEDETECTION_H
#define _LASERPEOPLEDETECTION_H

#include "laserPeopleCommon.h"
#include "BoostedCommittee.h"

#include <stdio.h> //fid=fopen
#include <string>  //path

//const double PERSON_RADIUS = 0.7;

//using namespace std;

/*!
  \class CLaserPeopleDetection
  \brief People detection from laser scan.
  Using a vector of points, detects people positions using a boosting classifier
  - Loads the classifier/s from files
  - Receives the data as a vector of ranges, angles minimum, maximum and resolution.
  - Uses ClaserPeopleCommon to:
    - Convert the data from Polar to Cartesian coordinates.
    - Make clusters of points (segments) using a jump distance threshold.
    - Compute a set of features to describe each segment.
  - Classifies each segment (set of features) as person leg or not.
  - Groups near legs in pairs
  - Fills people position list.
*/
class CLaserPeopleDetection
{
  protected:
    bool debug; //!< show debug output

    CLaserPeopleCommon Common; //!< common data processing (segments, features) class object
    CBoostedCommittee *Boost;  //!< boosting classifier class object
    CBoostedCommittee *Boost2; //!< boosting classifier class object 2

    std::string boostFilePath;      //!< path to boost classifier installed file   (/usr/local/include/iri/laser_people_detection/boostData)
    std::string boostFilePath2;     //!< path to boost classifier installed file 2 (/usr/local/include/iri/laser_people_detection/boostData)
    std::string localBoostFilePath; //!< relative path to local boost classifier (works only if executable is run from project subfolder)
    std::string boostFileString;    //!< name of boost classifier file (boostFile.txt)
    std::string boostFileString2;   //!< name of boost classifier file 2 (boostFile2.txt)
    FILE *fid;                 //!< file variable

    float range_threshold;     //!< threshold distance where to switch classifier [m][def 4]
    float detection_threshold; //!< theshold value where to classify data (positive when higher than threshold)

    std::vector<double> results;    //!< vector of detection values
    std::vector< std::vector<double> > people; //!< vector of people, where a person is a vector of x[m],y[m],detectionValue[0..1], covariance[m^2]

    double person_radius;  //!< radius where to pair legs corresponding to the same person

    /**
     * \brief Loads classifiers from installed txt files
     */
    bool loadBoostFiles();

    /**
     * \brief Uses boosting to classify segments (using theirs Common.features[]) storing results[]
     */
    void boostClassifier();

    /**
     * \brief Transforms from classified legs to people (pairs of legs), storing results in people[]
     */
    void locatePeople();

    /**
     * \brief Stores all positive segments (legs) in people[] (no pair-of-legs grouping, alternative to locatePeople, testing purposes only)
     */
    void locateLegs();

  public:
    CLaserPeopleDetection(); //!< constructor
    ~CLaserPeopleDetection(); //!< destructor

    /**
     * \brief Performs a detection iteration (Common.scanIteration + classify + locatePeople)
     */
    void detectionIteration(const std::vector<float> ranges, const float angle_min, const float angle_max, const float angle_increment, std::vector<std::vector<float> > * peopleList);

    /**
     * \brief Modifies the range_threshold used to change classifier
     */
    void setRangeThreshold(const float rangeThreshold);

    /**
     * \brief Modifies the detection_threshold used to classify segments
     */
    void setDetectionThreshold(const float detectionThreshold);

    /**
     * \brief Modifies the paths to the boost classifier files
     */
    void setBoostFilePaths(const std::string newBoostFilePath, const std::string newBoostFilePath2);

    /**
     * \brief Modifies the radius used to pair person legs
     */
    void setPersonRadius(const double personRadius);

    /**
     * \brief Modifies the jump distance used to split in segments
     */
    void setJumpDistance(const double jumpDistance);

    /**
     * \brief Modifies the minimum points number used to generate segments
     */
    void setMinPoints(const unsigned int minPoints);
};

#endif
