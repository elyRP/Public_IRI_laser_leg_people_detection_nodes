/*! \example laserPeopleDetection_test.cpp
In this example a file with scans data is loaded and people is detected.
Outputs people positions list.

\b Usage:
From build directory.

\code
../bin/laserPeopleDetection_test n /path/to/file.txt
\endcode

n=number of iterations

If not especified n=5 and path="../src/examples/offLineData/logFile.txt"
*/

#include "laserPeopleDetection.h"

#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
  int max_iter=5;
  std::string file="../src/examples/offLineData/logFile.txt";

  if(argc==1)
  {
    //defaults
    std::cout << "--- Usage: ./laserPeopleDetection_test"<< std::endl;
    std::cout << "---        ./laserPeopleDetection_test    iteration_number"<< std::endl;
    std::cout << "---        ./laserPeopleDetection_test    iteration_number   /path/to/logFile.txt"<< std::endl;
  }
  else if(argc==2)
  {
    max_iter=atoi(argv[1]);
    if(max_iter<=0)
    {
     std::cout << "!!! Error: invalid number of iterations: " << max_iter << std::endl;
     exit(1);
    }
  }
  else if(argc==3)
  {
    max_iter=atoi(argv[1]);
    if(max_iter==0)
    {
     std::cout << "!!! Error: invalid number of iterations: " << max_iter << std::endl;
     exit(1);
    }
    file = argv[2];
  }

  std::cout << "--- Running " << max_iter << " iterations on file: " << file << std::endl;
  
  // Initialize parameters associated to example scan
  int np;                 //682;
  float angle_min;       //2.094395103;
  float angle_max;       //2.094395103;
  float angle_increment; //0.006141921;
    
  CLaserPeopleDetection myPeopleD;
  myPeopleD.setJumpDistance(0.1);
  myPeopleD.setPersonRadius(0.7);

  // Open file to read from
  std::ifstream myfile(file.c_str());
  if (!myfile.is_open())
  {
    std::cout<<"!!! Error: couldn't open file \""<< file<<"\"." <<std::endl;
    exit(1);
  }
  else
  {
    std::cout <<"--- File opened." << std::endl;
    
    std::string line;

    //skip 2 comment lines
    std::getline(myfile,line);
    std::getline(myfile,line);

    //read parameters line
    std::getline(myfile,line);
    std::istringstream is( line );
    if(!(is >> np >> angle_min >> angle_max >> angle_increment))
    {
      std::cout << "!!! Error: couldn't read params from 3rd line." << std::endl;  
      exit(1);
    }
    std::cout << "--- Loaded parameters:";
    std::cout << "points = " <<np<<", ";
    std::cout << "angle_min = "<< angle_min <<", ";
    std::cout << "angle_max = "<< angle_max <<", ";
    std::cout << "angle_increment = "<< angle_increment;
    std::cout <<" ---"<< std::endl;
    
    std::vector<float> ranges(np,0.0);

    //skip comment line
    std::getline(myfile,line);

    std::cout << "--- Showing detected people for each iteration"<<std::endl;
    std::cout << "--- Person = ( x, y, probability, covariance )"<< std::endl;

    for(int h=0;h<max_iter;h++)
    {
      std::cout << "------- it: "<< h <<" -------"<< std::endl;

      if(!std::getline(myfile, line ))
      {
        std::cout << "!!! Warning: couldn't read next line. File may have reached the end. Exiting." << std::endl;  
        break;
      }
      std::istringstream is( line );
 
      float point;

      //skip first 3 numbers of each line (2, time, np)
      is>>point;
      is>>point;
      is>>point;

      unsigned int i=0;
      while(is>>point)
      {
        if(i<ranges.size())
        {
          ranges[i]=point;
          i++;
        }
        else
        {
          std::cout<<"!!! Error: iteration "<<h<<" has elements>"<<np<<". Skipping them."<<std::endl;
          break;
        }
      }

      // Perform detection iteration
      std::vector<std::vector<float> > peopleList;
      myPeopleD.detectionIteration(ranges,angle_min,angle_max,angle_increment, &peopleList);

      // Print detected people
      for(unsigned int i=0;i<peopleList.size(); i++)
      {
        std::cout << "   ( ";
        for (unsigned int j=0;j<peopleList[i].size(); j++)
          std::cout << peopleList[i][j] << " ";
        std::cout <<")"<< std::endl;
      }
    }
  }

  myfile.close();
  std::cout <<"--- File closed." << std::endl;
  
 return 0;
}
