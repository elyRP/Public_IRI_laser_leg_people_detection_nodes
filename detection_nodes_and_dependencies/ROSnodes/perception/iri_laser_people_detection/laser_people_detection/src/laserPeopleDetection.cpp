#include "laserPeopleDetection.h"

CLaserPeopleDetection::CLaserPeopleDetection()
{
  this->person_radius = 0.7;
  debug=false;
  //debug=Common.debug;
  range_threshold = 0.0;
  detection_threshold = -0.5;
  boostFilePath  = "/usr/local/include/lpd/boostData/";
  boostFilePath2 = "/usr/local/include/lpd/boostData/";
  localBoostFilePath = "../data/classifierData/";
  if(!loadBoostFiles())
  {
    std::cout << "!!! Error: can't load classifier files. Exiting " << std::endl;
    exit(0);
  }
}

CLaserPeopleDetection::~CLaserPeopleDetection()
{
}

bool CLaserPeopleDetection::loadBoostFiles()
{
  if(debug)
    std::cout << "CLaserPeopleDetection::loadBoostFiles()" << std::endl;

  Boost  = new CBoostedCommittee;
  Boost2 = new CBoostedCommittee;

  if(boostFilePath.substr(boostFilePath.size()-4,boostFilePath.size()) != ".txt")
  {
    boostFileString  = boostFilePath + "boostFile.txt";
  }
  else
  {
    boostFileString  = boostFilePath;
  }

  if(boostFilePath2.substr(boostFilePath2.size()-4,boostFilePath2.size()) != ".txt")
  {
    boostFileString2 = boostFilePath2 + "boostFile2.txt";
  }
  else
  {
    boostFileString2 = boostFilePath2;
  }

  // Loading files with learned values
  fid = fopen(boostFileString.c_str(),"r");
  if(fid==NULL)
  {
    std::cout << "!!! Error opening "<< boostFileString << std::endl;
    boostFileString = localBoostFilePath + "boostFile.txt";
    fid = fopen(boostFileString.c_str(),"r");
    if(fid==NULL)
    {
      std::cout << "!!! Error opening "<< boostFileString << std::endl;
      return false;
    }
    else
      std::cout << "--- Loaded " << boostFileString << std::endl;
  }
  else
    std::cout << "--- Loaded " << boostFileString << std::endl;
    
  // Loading classifier
  if(! Boost->LoadFromFile(fid)) std::cout << "---Error reading " << boostFileString << std::endl;
  else   std::cout << "--- Read " << boostFileString << std::endl;
  fclose (fid);

  fid = fopen(boostFileString2.c_str(),"r");
  if(fid==NULL)
  {
    std::cout << "!!! Error opening "<< boostFileString2 << std::endl;
    boostFileString2 = localBoostFilePath + "boostFile2.txt";
    fid = fopen(boostFileString2.c_str(),"r");
    if(fid==NULL)
    {
      std::cout << "!!! Error opening "<< boostFileString2 << std::endl;
      return false;
    }
    else
      std::cout << "--- Loaded " << boostFileString2 << std::endl;
  }
  else
    std::cout << "--- Loaded " << boostFileString2 << std::endl;
  // Loading classifier
  if(! Boost2->LoadFromFile(fid)) std::cout << "---Error reading " << boostFileString2 << std::endl;
  else   std::cout << "--- Read " << boostFileString2  << std::endl;
  fclose (fid);
  return true;
}

void CLaserPeopleDetection::boostClassifier()
{
  if(debug)
    std::cout << "CLaserPeopleDetection::boostClassifier()" << std::endl;

  double fff[NUMBER_OF_FEATURES];

  results.assign (Common.features.size(),1.0);

  for(unsigned int i=0; i<Common.features.size(); i++)
  {
    for(unsigned int j=0; j<Common.features[0].size(); j++)
    {
      fff[j]=Common.features[i][j];
    }

    // disti: distance between first segment point and laser
    float xi = Common.points[Common.segments[i].first -1].first;
    float yi = Common.points[Common.segments[i].first -1].second;
    float disti = sqrt(pow(xi,2)+pow(yi,2));

    // Using classifier, the signum of output is the predicted class {-1,+1}
    if(disti<range_threshold) // Choose classifier depending on range
    {
      results[i] = Boost->Predict(fff);
    }
    else
    {
      results[i] = Boost2->Predict(fff);
    }
  }

  if(debug)
    std::cout << "results.size(): " << results.size() << std::endl;

}

void CLaserPeopleDetection::locatePeople()
{
  if(debug)
    std::cout << "CLaserPeopleDetection::locatePeople(), Common.segments.size()=" <<Common.segments.size()<< std::endl;

  std::vector<int>    np (Common.segments.size(),0.0);
  std::vector<double> xm (Common.segments.size(),0.0); //x means of each segment
  std::vector<double> ym (Common.segments.size(),0.0); //y means of each segment
  std::vector<double> rm (Common.segments.size(),0.0); //quality value of each segment
  std::vector<double> dev(Common.segments.size(),0.0); //deviation of each segment
  std::vector<double> om (Common.segments.size(),0.0); //state of each segment, 0=unused, 1=1validsegment, 2=2validsegments -1=added to another segment

  //PERSON_RADIUS=0.7

  for(unsigned int k=0; k<results.size(); k++)
  {
    if(debug){std::cout << "(1) CLaserPeopleDetection::locatePeople() results["<< k <<"]=" <<results[k]<< std::endl;}
    rm[k]=results[k];
  }

  double n;
  double x;
  double y;

  for(unsigned int s=0; s<Common.segments.size(); s++) // for each segment, calculate x mean, y mean, and number of points
  {

    // save current segment data (np, xm, ym)
    n = Common.segments[s].second-Common.segments[s].first + 1;
    np[s] = n;
    x=0;
    y=0;
    for(int m=Common.segments[s].first-1; m<Common.segments[s].second; m++)
    {
      x += Common.points[m].first;
      y += Common.points[m].second;
    }
    xm[s] = x / np[s];
    ym[s] = y / np[s];

    //for(int m=Common.segments[s].first-1; m<Common.segments[s].second; m++)
    //{
    //  dev[s]+= pow((Common.points[m].first - xm[s]),2) + pow((Common.points[m].second -ym[s]),2);
    //}
    //dev[s]=sqrt(dev[s]/(np[s]-1));
    dev[s]= Common.getSegmentDeviation(s);
    //std::cout << "dev: " << dev[s] << std::endl;
  }

  for(unsigned int s=0; s<Common.segments.size(); s++) // for each segment
  {
    //values of current segment
    n=np[s];
    x=xm[s]*np[s];
    y=ym[s]*np[s];
if(debug){std::cout << " (LAST) CLaserPeopleDetection::locatePeople(), rm<[s]="<<rm[s]<<">"<<"detection_threshold"<<detection_threshold<<"om[s]="<<om[s]<< std::endl;}

   /*if(0.5 > 0.1){
    std::cout << " entro if test!!! "<< std::endl;
   }

   if(- 0.08 > - 0.009){
    std::cout << " entro if test 2!!! "<< std::endl;
   }*/

    if((rm[s]>detection_threshold) && (om[s]==0)) // if its positive (leg), and still not used (om=0)
    {
    if(debug){std::cout << " (LAST) CLaserPeopleDetection::locatePeople(), IN if!!! "<< std::endl;}
      om[s]=1; // already used, valid positive segment
      for(unsigned t=0; t<Common.segments.size(); t++) // for each other segments
      {
         
        //adding positive or little ones
        if(om[t]==0 && (rm[t]>detection_threshold || np[t]<4 ) ) // if it is still not valid (avoid already valid segments (+1) and segments already grouped to others (-1))
        {
          double dist = sqrt( pow( xm[s]-xm[t], 2) + pow( ym[s]-ym[t], 2) ); // distance between segments
          if(dist<this->person_radius && dist!=0) // [t] segment near [s] segment, and t!=s (dist!=0)
          {
            n = n + np[t]; // add segment (sum points to previous)
            for(int m=Common.segments[t].first-1; m<Common.segments[t].second; m++) //add x,y values to previous ones
            {
              x += Common.points[m].first;
              y += Common.points[m].second;
            }
            if(results[t]>detection_threshold && om[s]==1)
            {
              om[s]=2; // consists in 2 positive segments 
              //TODO: choose how to compute probability of leg pair
              rm[s]=rm[s]+rm[t]; // add the quality value if we are adding a positive segment (only once, om[s] max = 2, 2 legs)
              //rm[s]=(rm[s]+rm[t])/2.0; //average the quality
            }
            xm[s]=x/n; // update gravity center of the group
            ym[s]=y/n;
            om[t]=-1; // already used, segment grouped because of proximity (positive or not)
          }
        }
      }
    }
  }
  people.clear();
    if(debug){std::cout << " (LAST) CLaserPeopleDetection::locatePeople(), Common.segments.size()=" <<Common.segments.size()<< std::endl;}

  for(unsigned int s=0; s<Common.segments.size(); s++)
  { if(debug){std::cout << " (LAST) CLaserPeopleDetection::locatePeople() iom[s]="<<om[s]<< std::endl;}
    if(om[s]==1 || om[s]==2) //if valid segment or segments
    {
    if(debug){std::cout << " (LAST) CLaserPeopleDetection::locatePeople() in valid segment"<< std::endl;}
      std::vector<double> person(4); //[x,y,quality,covariance]
      person[0]= xm[s];
      person[1]= ym[s];
      //TODO: choose how to compute probability scale
      //person[2]= rm[s];
      person[2]=(rm[s]-this->detection_threshold)/(Boost->getMaxWeight()-this->detection_threshold); //normalized value(0..1) = (value - min)/(max - min)
      //TODO: choose how to compute covariance
      person[3]= 2*dev[s]*dev[s];
      people.push_back(person);
    }
  }
  if(debug)
    std::cout << "people.size(): " << people.size() << std::endl;
}

void CLaserPeopleDetection::locateLegs()
{
  if(debug)
    std::cout << "CLaserPeopleDetection::locateLegs()" << std::endl;

  std::vector<int>    np (Common.segments.size(),0.0);
  std::vector<double> xm (Common.segments.size(),0.0);
  std::vector<double> ym (Common.segments.size(),0.0);
  std::vector<double> rm (Common.segments.size(),0.0);

  for(unsigned int k=0; k<results.size(); k++)
  {
   if(debug){
   std::cout << "(1) CLaserPeopleDetection::locateLegs() results["<< k <<"]=" <<results[k]<< std::endl;}
    rm[k]=results[k];
  }

  people.clear();

  for(unsigned int s=0; s<Common.segments.size(); s++) // for each segment
  {
    if(debug){
    std::cout << "(2) CLaserPeopleDetection::locateLegs() results["<< s <<"]=" <<results[s]<< std::endl;}
    if(results[s]>detection_threshold) // if its positive (leg)
    {
      double n2=0;
      double x2=0;
      double y2=0;
      // save current segment data (np, xm, ym)
      n2 = Common.segments[s].second-Common.segments[s].first + 1;
      np[s] = n2;

      for(int m=Common.segments[s].first-1; m<Common.segments[s].second; m++)
      {
        x2 += Common.points[m].first;
        y2 += Common.points[m].second;
      }

      xm[s] = x2 / np[s];
      ym[s] = y2 / np[s];

      std::vector<double> leg(4);
      leg[0]= xm[s];
      leg[1]= ym[s];
      leg[2]= rm[s];
      leg[3]= 0.001;

      people.push_back(leg);
      leg.clear();
    }
  }

  if(debug)
    std::cout << "people.size(): " << people.size() << std::endl;

}

void CLaserPeopleDetection::detectionIteration(const std::vector<float> ranges, const float angle_min, const float angle_max, const float angle_increment, std::vector<std::vector<float> > * peopleList)
{
 if(debug){std::cout << "IN internal lib detectionIteration" << std::endl;}
  Common.scanIteration(ranges, angle_min, angle_max, angle_increment);
  boostClassifier();
  locatePeople();
  //locateLegs();

  std::vector<float> person(4,0);
  for(unsigned int i=0; i<people.size(); i++)
  {
    for(unsigned int j=0; j<people[0].size(); j++)
    {
      person[j]=people[i][j];
    }
    peopleList->push_back(person);
  }

}

void CLaserPeopleDetection::setRangeThreshold(const float rangeThreshold)
{
  if(debug)
    std::cout << "CLaserPeopleDetection::setRangeThreshold()" << std::endl;

  range_threshold = rangeThreshold;
}

void CLaserPeopleDetection::setDetectionThreshold(const float detectionThreshold)
{
  if(debug)
    std::cout << "CLaserPeopleDetection::setdetectionThreshold" << std::endl;

  if(detectionThreshold!=this->detection_threshold)
    std::cout << "CLaserPeopleDetection::setDetectionThreshold: detection threshold set to: "<< detectionThreshold << std::endl;

  if(detectionThreshold < Boost->getMinWeight())
    this->detection_threshold = Boost->getMinWeight();
  else
    this->detection_threshold = detectionThreshold;
}

void CLaserPeopleDetection::setBoostFilePaths(const std::string newBoostFilePath, const std::string newBoostFilePath2)
{
  if(debug)
    std::cout << "CLaserPeopleDetection::setBoostFilePaths()" << std::endl;

  bool refresh=0;

  if(boostFilePath != newBoostFilePath)
  {
    boostFilePath = newBoostFilePath;
    refresh=1;
  }
  if(boostFilePath2 != newBoostFilePath2)
  {
    boostFilePath2 = newBoostFilePath2;
    refresh=1;
  }
  if(refresh)
    loadBoostFiles();
}

void CLaserPeopleDetection::setPersonRadius(const double personRadius)
{
  if(debug)
    std::cout << "CLaserPeopleDetection::setPersonRadius" << std::endl;
  if(this->person_radius!=personRadius)
  {
    this->person_radius = personRadius;
    if(debug){std::cout << "CLaserPeopleDetection::setPersonRadius: person radius set to: "<< this->person_radius << std::endl;}
  }
}

void CLaserPeopleDetection::setJumpDistance(const double jumpDistance)
{
  if(debug)
    std::cout << "CLaserPeopleDetection::setJumpDistance" << std::endl;

  Common.setJumpDistance(jumpDistance); 
}

void CLaserPeopleDetection::setMinPoints(const unsigned int minPoints)
{
  if(debug)
    std::cout << "CLaserPeopleDetection::setMinPoints" << std::endl;

  Common.setMinPoints(minPoints); 
}
