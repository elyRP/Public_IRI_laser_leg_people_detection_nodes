#ifndef _LASERPEOPLELABEL_H
#define _LASERPEOPLELABEL_H

#include "laserPeopleCommon.h"

#include <fstream>   //ofstream file_out
#include <algorithm> // pair< int,int>

//using namespace std;

/*!\class CLaserPeopleLabel
  \brief Class with methods for manual data labeling.
  To be used by the ros node iri_laser_people_label
*/
class CLaserPeopleLabel
{
  protected:
    bool debug; //!< show debug output

    CLaserPeopleCommon Common; //!< common data processing (segments, features) class object

    std::ofstream file_out;  //!< output file 1
    std::ofstream file_out2; //!< output file 2

    std::vector<float> ranges;  //!< vector of point ranges [m]
    float angle_min;       //!< start angle of the scan [rad]
    float angle_max;       //!< end angle of the scan [rad]
    float angle_increment; //!< angular distance between measurements [rad]
    float jump_distance;   //!< threshold distance between points to generate a new segment [m]

    std::vector< std::pair<int,int> > labels; //!< vector of pairs with iteration and number of segment labeled
    std::vector<float> intensities;      //!< vector of intensities telling if a point pertains to a labeled segment (1) or not (0)

    /**
     * \brief Clears vector variables (labels and intensities)
     */
    void initialize();

    /**
     * \brief Given a Cartesian point (goal), returns the nearest segment index and its first and last point indexs.
     */
    void nearestSegment(const std::vector<float> goal, int * goalSegment, int * firstPoint, int * lastPoint);

  public:
    CLaserPeopleLabel(float jump_distance);  //!< constructor
    ~CLaserPeopleLabel(); //!< destructor

    std::string filePath;   //!< path to files
    std::string pointsFile; //!< laser points file name
    std::string labelsFile; //!< segment labels file name
    std::string paramsFile; //!< laser params file name
    int lastIter; //!< last iteration index

    /**
     * \brief Save given parameters to file
     * \param angleMin angle of the first point in the scan
     * \param angleMax angle of the last point in the scan
     * \param angleIncrement resolution, angle between consecutive points in the scan
     */
    void saveParamsFile(const float angleMin, const float angleMax, const float angleIncrement);

    /**
     * \brief Save given points to file, appending them to existent file or overwritting it
     * \param inRanges vector of ranges
     * \param append append data to file or not
     */
    void savePointsFile(const std::vector<float> inRanges, bool append);

    /**
     * \brief Save current labels to file, appending them to existent file or overwritting it
     * \param append append data to file or not
     */
    void saveLabelsFile(bool append);
    
    /**
     * \brief Given points and params, loads iteration (point extraction and segmentation)
     * \param inRanges vector of ranges
     * \param angleMin angle of the first point in the scan
     * \param angleMax angle of the last point in the scan
     * \param angleIncrement resolution, angle between consecutive points in the scan
     */
    void labelIteration(const std::vector<float> inRanges, const float angleMin, const float angleMax, const float angleIncrement);

    /**
     * \brief Given a point, adds or removes nearest segment, updating labels and intensities
     * \param goal Cartesian point (x,y)
     * \param currentIteration current iteration index
     * \return the first point of the segment
     */
    int  labelSegment(std::vector<float> goal, int currentIteration);

    /**
     * \brief Given area coordinates and current iteration, auto labels segments in the area
     * \param xmax maximum x coordinate
     * \param xmin minimum x coordinate
     * \param ymax maximum y coordinate
     * \param ymin minimum y coordinate
     * \param sign adding or substracting area
     * \param currentIteration current iteration index
     */
    void labelArea(float xmax, float xmin, float ymax, float ymin, int sign, int currentIteration);

    /**
     * \brief Remove labels associated to currentIteration
     * \param currentIteration current iteration index
     */
    void removeLabels(int currentIteration);

    /**
     * \brief Gives number of scans present in points file
     * \param fileString file path
     * \return number of scans
     */
    int  getNumberOfScans(std::string fileString);

    /**
     * \brief Gives intensities
     * \return intensisties vector
     */
    std::vector<float> getIntensities();

    /**
     * \brief Sets new path for files
     * \param newFilePath new paths of files
     */
    void setFilePath(std::string newFilePath);

};

#endif
