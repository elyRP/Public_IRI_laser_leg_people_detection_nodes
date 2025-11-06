#include "laserPeopleLabel.h"

CLaserPeopleLabel::CLaserPeopleLabel(float jump_distance)
{
  debug=this->Common.debug;
  //debug=false;

  //std::string homepath = getenv("HOME");
  filePath = "../data/";
  pointsFile = filePath + "points.txt";
  labelsFile = filePath + "labels.txt";
  paramsFile = filePath + "params.txt";
  this->jump_distance = jump_distance;
  this->Common.setJumpDistance(this->jump_distance); 
  lastIter = 0;
}

CLaserPeopleLabel::~CLaserPeopleLabel()
{
}

void CLaserPeopleLabel::initialize()
{
  if(debug)
    std::cout << "CLaserPeopleLabel::initialize" << std::endl;
  labels.clear();
  intensities.clear();
}

void CLaserPeopleLabel::saveParamsFile(const float angleMin, const float angleMax, const float angleIncrement)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::saveParamsFile" << std::endl;

  angle_min       = angleMin;
  angle_max       = angleMax;
  angle_increment = angleIncrement;

  file_out2.open (paramsFile.c_str());
  if (file_out2.is_open())
  {
    file_out2 << angleMin;
    file_out2 << " ";
    file_out2 << angleMax;
    file_out2 << " ";
    file_out2 << angleIncrement;
    file_out2 << " ";
    file_out2 << this->jump_distance;
    file_out2 << "\r\n";
  }
  else
  {
    std::cout << "!!! Error opening "<< paramsFile << " to write on it"<<std::endl;
    exit(1);
  }
  file_out2.close();
}

void CLaserPeopleLabel::savePointsFile(const std::vector<float> inRanges, bool append)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::savePointsFile" << std::endl;

  ranges=inRanges;
  intensities.clear();
  intensities.resize(ranges.size(),0.0);

  if(!file_out.is_open())
  {
    if(!append)
    {
      file_out.open (pointsFile.c_str());
    }
    else
    {
      file_out.open (pointsFile.c_str(),std::ios::app);
    }
  }
  if (file_out.is_open())
  {
    for(unsigned int i=0; i<ranges.size(); i++)
    {
      file_out << ranges[i];
      file_out << " ";
    }
    file_out << "\r\n";
  }
  else
  {
    std::cout << "!!! Error opening " << pointsFile << " to write on it"<<std::endl;
    exit(1);
  }
  //file_out.close(); //closed in Labeler
}

void CLaserPeopleLabel::saveLabelsFile(bool append)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::saveLabelsFile" << std::endl;

  file_out.close(); //close opened file for pointsFile
  if(append==0)
  {
    file_out.open(labelsFile.c_str());
  }
  else
  {
    file_out.open(labelsFile.c_str(),std::ios::app);
  }
  if (file_out.is_open())
  {
    for(unsigned int i=0; i<labels.size(); i++)
    {
      file_out << labels[i].first+lastIter;
      file_out << " ";
      file_out << labels[i].second;
      file_out << " ";
      file_out << "\r\n";
    }
  }
  else
  {
    std::cout << "!!! Error opening " << labelsFile << " to write on it" << std::endl;
    exit(1);
  }
  file_out.close();
  std::cout << "--- Written " << labelsFile <<std::endl;
  std::cout << "--- Use generated files (params.txt, points.txt, labels.txt) to train the classifier and generate the boostFile.txt" << std::endl;
  std::cout << "--- To do the training, run ./iri-lab/labrobotica/algorithms/laser_people_train/trunk/bin/laserPeopleTrain_test numberOfIterations \"pathToFiles\"" << std::endl;
}

void CLaserPeopleLabel::labelIteration(const std::vector<float> inRanges, const float angleMin, const float angleMax, const float angleIncrement)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::labelIteration" << std::endl;
  //saveParamsFile(angleMin, angleMax, angleIncrement);
  //savePointsFile(inRanges, append);
  ranges=inRanges;
  intensities.clear();
  intensities.resize(inRanges.size(),0.0);
  this->Common.scanIteration2(inRanges, angleMin, angleMax, angleIncrement);
}

int CLaserPeopleLabel::labelSegment(std::vector<float> goal, int currentIteration)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::labelSegment" << std::endl;
  //Get nearest segment to goal
  int goalSegment;
  int firstPoint;
  int lastPoint;
  nearestSegment(goal, &goalSegment, &firstPoint, &lastPoint);

  //Find if the segment was already labeled
  bool removeSegment=false;
  int line2remove=-1;
  for(unsigned int i=0; i<labels.size(); i++)
  {
    if(labels[i].first==currentIteration && labels[i].second==goalSegment+1)
    {
      removeSegment=true;
      line2remove=i;
      break;
    }
  }

  //Add or remove line in labelsFile
  if(removeSegment)
  {
    std::cout<<"--- Removed segment "<< goalSegment+1 << std::endl;
    labels.erase(labels.begin()+line2remove);
  }
  else if (!removeSegment)
  {
    std::cout<<"--- Added   segment "<< goalSegment+1 << std::endl;
    std::pair<int,int> label;
    label.first  = currentIteration;
    label.second = goalSegment+1;
    labels.push_back(label);
  }

//Update intensities
  for(unsigned int i=0; i<ranges.size(); i++)
  {
    int ii=i;
    if(ii>=firstPoint-1 && ii<=lastPoint-1)
    { //for points from the segment added/removed
      if(intensities[i]==0)
      {
        intensities[i]=1; // switch intensity between 1 and 0
      }
      else if(intensities[i]==1)
      {
        intensities[i]=0; // switch intensity between 1 and 0
      }
    }
  }

return firstPoint;

}

void CLaserPeopleLabel::nearestSegment(const std::vector<float> goal, int * goalSegment, int * firstPoint, int * lastPoint)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::nearestSegment" << std::endl;
  float minDistance=9e9;
  int nearestPoint=-1;
  *firstPoint=-1;
  *lastPoint = -1;
  for(unsigned int i=0; i<this->Common.points.size(); i++)
  {
   float x = this->Common.points[i].first;
   float y = this->Common.points[i].second;
   float x0 = goal[0];
   float y0 = goal[1];

   float distance = sqrt(pow((x0-x),2)+pow((y0-y),2));
   if(distance<minDistance)
    {
     minDistance=distance;
     nearestPoint=i;
    }
  }

  *goalSegment=-1;
  for(unsigned int s=0; s<this->Common.segments.size(); s++)
  {
    if(nearestPoint+1<=this->Common.segments[s].second)
    {
      *goalSegment=s;
      *firstPoint=this->Common.segments[s].first;
      *lastPoint =this->Common.segments[s].second;
      break;
    }
  }

}

void CLaserPeopleLabel::labelArea(float xmax, float xmin, float ymax, float ymin, int sign, int currentIteration)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::labelArea" << std::endl;

  //std::cout <<"Area xmax,xmin,ymax,ymin: " <<  xmax << " " << xmin << " " <<ymax << " " << ymin << std::endl;
  for(unsigned int i=0; i<this->Common.segments.size(); i++)
  {
    float xm;
    float ym;
    xm=0.0;
    ym=0.0;
    for(int j=this->Common.segments[i].first-1; j<=this->Common.segments[i].second-1; j++)
    {
      xm+=this->Common.points[j].first;
      ym+=this->Common.points[j].second;
    }

    int np = this->Common.segments[i].second - this->Common.segments[i].first + 1;
    xm=xm/np;
    ym=ym/np;

    int goalSegment=-1;
    //std::cout << "s,np,xm,ym: "<< i+1<< " " <<np << ";"<< xm<< "," << ym << std::endl;

    if(xm<xmax && xm>xmin && ym<ymax && ym>ymin) //inside square
    {
      goalSegment=i;
      //std::cout << "goalSegment: " << goalSegment+1 << std::endl;
      //std::cout << "s,np,xm,ym: "<< i+1<< " " <<np << ";"<< xm<< "," << ym << std::endl;
    }

    // Labels: add or remove line [it,s]
    bool removeSegment=false;


    int lineToRemove=-1;
    for(unsigned int i=0; i<labels.size(); i++)
    {
      if((labels[i].first==currentIteration) && (labels[i].second==goalSegment+1))
      {
        removeSegment=true;
        lineToRemove=i;
      }
    }

    if(sign<0)
    {
      removeSegment=true;
    }
    else if(sign>0)
    {
      removeSegment=false;
    }

    //Add or remove line in labels
    if(removeSegment && goalSegment!=-1 && lineToRemove!=-1)
    {
      std::cout<<"--- Area: removed segment "<< goalSegment+1 << std::endl;
      labels.erase(labels.begin()+lineToRemove);
    }
    else if (!removeSegment && goalSegment!=-1 && lineToRemove==-1)
    {
      std::cout<<"--- Area: added segment "<< goalSegment+1 << std::endl;
      std::pair<int,int> label;
      label.first  = currentIteration;
      label.second = goalSegment+1;
      labels.push_back(label);
    }
  }
}

void CLaserPeopleLabel::removeLabels(int currentIteration)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::removeLabels" << std::endl;

  int lines2Remove=0;

  if(currentIteration==1)
  {
    labels.clear();
    return;
  }

  if(labels.size()>0)
  {
    for(unsigned int i=labels.size()-1; i>=0; i--)
    {
      if( labels[i].first==currentIteration ) // while matching iteration, is a line to be removed
      {
        lines2Remove++;
      }
      else
      {
      break; //when stops matching, nothing more to search
      }
    }
  }

  // Remove last rows (much as linesToRemove)
  if(lines2Remove>0)
  {
    labels.erase(labels.end()-lines2Remove, labels.end());
  }
}

int CLaserPeopleLabel::getNumberOfScans(std::string fileString)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::getNumberOfScans" << std::endl;

  std::ifstream inData;
  inData.open( fileString.c_str() );
  if ( !inData )
  {
    std::cout<<"!!! Error opening "<< pointsFile << " to get number of scans"<<std::endl;
    exit(1);
  }
  int size = 0;
  std::string line;
  while( getline(inData, line) )
  {
    size++;
  }
  lastIter = size;
  return size;
}

std::vector<float> CLaserPeopleLabel::getIntensities()
{
  if(debug)
    std::cout << "CLaserPeopleLabel::getIntensities" << std::endl;

  return intensities;
}

void CLaserPeopleLabel::setFilePath(std::string newFilePath)
{
  if(debug)
    std::cout << "CLaserPeopleLabel::setFilePath" << std::endl;

  filePath = newFilePath;
  pointsFile = filePath + "points.txt";
  labelsFile = filePath + "labels.txt";
  paramsFile = filePath + "params.txt";
}