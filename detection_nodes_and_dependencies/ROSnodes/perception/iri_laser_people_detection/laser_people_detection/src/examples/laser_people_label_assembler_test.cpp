/*! \example laserPeopleLabelAssembler_test.cpp
Test to fusion pairs of labeled data sets, to perform a single training with both.
 
\b Usage
\code
./laserPeopleLabelAssembler_test /path/to/files
\endcode

At least 2 sets of files from previous labelings are needed, stored in folders in the provided path/to/files
Default path is ~/iri-lab/ros/iri-ros-pkg/iri_perception/iri_laser_people_label/data/

\code
/path/to/files/folder1/params.txt
/path/to/files/folder1/points.txt
/path/to/files/folder1/labels.txt
/path/to/files/folder2/params.txt
/path/to/files/folder2/points.txt
/path/to/files/folder2/labels.txt
\endcode

After introducing the folder names f1 and f2, the data will be fusioned and stored in a new folder f1f2
\code
/path/to/files/f1f2/params.txt
/path/to/files/f1f2/points.txt
/path/to/files/f1f2/labels.txt
\endcode

Ready to be used by the trainer (see labrobotica/algorithms/laser_people_train)
*/

#include "laserPeopleLabel.h"

// Returns if specified file exists
bool doFileExist(std::string fileStringName)
{
  std::ifstream ifile(fileStringName.c_str());
  return ifile;
}

int getIterFromDataFile(std::string fileString)
{
  //std::string dataFileString = "/home/fherrero/iri-lab/ros/iri-ros-pkg/iri_perception/iri_laser_people_label/data/points.txt";
  //Get last iteration
  std::ifstream inData;
  inData.open( fileString.c_str() );
  if ( !inData )
  {
    std::cout<<"!!! Error opening "<< fileString << " to get number of scans"<<std::endl;
    exit(1);
  }
  int size = 0;
  std::string line;
  while( std::getline(inData, line) )
  {
    size++;
  }
  int lastIter = size;
  return lastIter;
}

int main(int argc, char *argv[])
{
  std::string pathString;

  std::cout << std::endl;
  std::cout << "---------------------" << std::endl;
  std::cout << "   LABEL ASSEMBLER   " << std::endl;
  std::cout << "---------------------" << std::endl;
  std::cout << std::endl;
  std::cout << "   Uses 2 sets of labeled files (params.txt, points.txt, labels.txt) located in two different folders f1,f2 (names asked)" << std::endl;
  std::cout << "   Generates 1 set of labeled files (params.txt, points.txt, labels.txt) located in a new folder f1f2" << std::endl;
  std::cout << std::endl;
  std::cout << "---------------------" << std::endl;
  std::cout << std::endl;
  
  std::string homePath = getenv("HOME");
  std::string defaultPath = homePath + "/iri-lab/labrobotica/algorithms/laser_people_train/trunk/data/";

  if(argc==1)
  {
    pathString= defaultPath;
    std::cout << "--- Default path for folders/files: " << pathString << std::endl;
  }
  else if(argc==2)
  {
    pathString= argv[1];
    std::cout << "--- Set path for folders/files: " << pathString << std::endl;
  }
  else
  {
    std::cout << "!!! Error Invalid arguments. See usage"<<std::endl;
    exit(1);
  }

  int lastIter=0;

  std::string name1="params.txt";
  std::string name2="points.txt";
  std::string name3="labels.txt";

  std::string path11, path12, path13, path21, path22, path23;
  std::string input;
  std::string input2;

  std::string newFolder;

  while(1)
  {
    std::cout << "--- Enter FIRST  folder name with files to combine: ";
    std::getline( std::cin, input );
    std::cout << "--- Enter SECOND folder name with files to combine: ";
    std::getline( std::cin, input2 );

    newFolder = input + input2;

    input  = pathString + "/" + input;
    input2 = pathString + "/" + input2;

    path11=input  +"/"+name1;
    path12=input  +"/"+name2;
    path13=input  +"/"+name3;
    path21=input2 +"/"+name1;
    path22=input2 +"/"+name2;
    path23=input2 +"/"+name3;

    bool correct1 = doFileExist(path11) && doFileExist(path12) && doFileExist(path13);
    bool correct2 = doFileExist(path21) && doFileExist(path22) && doFileExist(path23);
    if(correct1 && correct2){
      //do combine
      break;
    }
    else {
      std::cout << "--- Invalid paths or missing folders/files in "<< pathString << ". Try again" << std::endl;
    }
  }



  std::string mkdirCommand = "mkdir -p " + pathString + "/" + newFolder;
  int notOk = system(mkdirCommand.c_str());
  if(notOk!=0){
   std::cout<<"--- Could not create dir \""+newFolder+"\" for combined files creation" << std::endl;
   exit(1);
  }
  else
  {
    std::cout << "--- Created new folder for assembled files, named: " + newFolder << std::endl;
  }

  std::ifstream file1, file2;
  //Open and compare params.txt
  std::ofstream fileout;
  std::string output=pathString+"/"+newFolder+"/"+name1;

  fileout.open(output.c_str());
  if( fileout.is_open() ) {
    file1.open(path11.c_str());
    file2.open(path21.c_str());
    std::vector<float> values1(3);
    std::vector<float> values2(3);
    if ( file1.is_open() && file2.is_open() ){
      std::string line;
      std::getline(file1, line);
      std::stringstream iss(line);
      iss >> values1[0];
      iss >> values1[1];
      iss >> values1[2];
      std::getline(file2,line);
      std::stringstream iss2(line);
      iss2 >> values2[0];
      iss2 >> values2[1];
      iss2 >> values2[2];
      if(values1 != values2){
    std::cout << "--- You are combining data with different parameters!"<<std::endl;
    exit(1);
      }
      fileout << values1[0];
      fileout << " ";
      fileout << values1[1];
      fileout << " ";
      fileout << values1[2];
      fileout << "\r\n";
    }
    else {
      std::cout << "!!! Error opening params.txt files" << std::endl;
      exit(1);
    }
    file1.close();
    file2.close();
  }
  else {
    std::cout << "!!! Error opening params.txt to write on it"<<std::endl;
    exit(1);
  }
  std::cout << "--- Writen combined params.txt in folder " + newFolder << std::endl;
  fileout.close();

  //Open and combine data.txt
  output=pathString+"/"+newFolder+"/"+name2;

  fileout.open(output.c_str());
  if( fileout.is_open() ){
    file1.open(path12.c_str());
    if ( file1.is_open() ){
      std::string line;
      int i=0;
      while( std::getline(file1, line) ){
        fileout << line;
        fileout << "\n";
        i++;
      }
      std::cout<<"--- Data1: " << i << " lines" << std::endl;
    }
    else {
      std::cout << "!!! Error opening data.txt file 1" << std::endl;
      exit(1);
    }
    file1.close();

    file2.open(path22.c_str());
    if ( file2.is_open() ) {
      std::string line;
      int i=0;
      while( std::getline(file2, line) ){
        fileout << line;
        fileout << "\n";
        i++;
      }
      std::cout<<"--- Data2: " << i << " lines" << std::endl;
    }
    else {
      std::cout << "!!! Error opening data.txt file 2" << std::endl;
      exit(1);
    }
    file2.close();

  }
  else {
    std::cout << "!!! Error opening data.txt to write on it"<<std::endl;
    exit(1);
  }
  std::cout << "--- Writen combined data.txt in folder " + newFolder << std::endl;
  fileout.close();

  lastIter = getIterFromDataFile(output.c_str());
  std::cout<<"--- Data3: " << lastIter << " lines" << std::endl;

  //Open and combine labels.txt
  output=pathString+"/"+newFolder+"/"+name3;

  fileout.open(output.c_str());
  if( fileout.is_open() ) {
    lastIter = getIterFromDataFile(path12.c_str());
    //int lastIter= myPeopleLabeler->lastIter;
    file1.open(path13.c_str());
    if ( file1.is_open() ) {
      std::string line;
      while( std::getline(file1, line) ){
    fileout << line;
    fileout << "\n";
      }
    }
    else {
      std::cout << "!!! Error opening labels.txt file 1" << std::endl;
      exit(1);
    }
    file1.close();

    file2.open(path23.c_str());
    if ( file2.is_open() ) {
      std::string line;
      while (std::getline(file2, line)) {
    int value;
    std::stringstream iss(line);
    int j=0;
    while (iss >> value) {
      if(j==0){
        fileout<<value+lastIter;
        fileout<<" ";
      }
      else if(j==1){
       fileout<<value;
       fileout<<" \n";
      }
      j++;
    }
      }
    }
    else {
      std::cout << "!!! Error opening labels.txt file 2" << std::endl;
      exit(1);
    }
    file2.close();

  }
  else {
    std::cout << "!!! Error opening labels.txt to write on it"<<std::endl;
    exit(1);
  }
  std::cout << "--- Writen combined labels.txt in folder " + newFolder << std::endl;
  fileout.close();
}