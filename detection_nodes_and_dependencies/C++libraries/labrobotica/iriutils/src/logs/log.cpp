// Copyright (C) 2009-2010 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
// Author Sergi Hernandez  (shernand@iri.upc.edu)
// All rights reserved.
//
// This file is part of iriutils
// iriutils is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "log.h"
#include "logexceptions.h"

const std::string path = "./log/";
const std::string ext  = ".log"; 

CLog::CLog(const std::string& filename)
{
  struct stat st;
  int error=0;
  
  if (filename.empty())
    throw CLogException(_HERE_,"Empty filename for log","");
 
  error=stat(path.c_str(),&st);
  if(error!=0)
  {
    if(mkdir(path.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)!=0)
      throw CLogException(_HERE_,"Impossible to create log folder",path);
  }
  this->filename=path + filename + ext;
  this->log_file.open(this->filename.c_str(),std::ios_base::out);

  if (! this->log_file)
    throw CLogException(_HERE_,"Log file can not be open",this->filename);

  this->enabled = true;
  this->set_time_format(ctf_datetime);
}

void CLog::enable(void)
{
  this->access.enter();
  this->enabled=true;
  this->access.exit();
}

void CLog::disable(void)
{
  this->access.enter();
  this->enabled=false;
  this->access.exit();
}

bool CLog::is_enabled(void)
{
  return this->enabled;
}

std::string CLog::get_filename(void)
{
  return this->filename;
}

void CLog::set_time_format(ctimeformat format)
{
  this->time_stamp.setFormat(format);
}

ctimeformat CLog::get_time_format()
{
  return this->time_stamp.getFormat();
}

CLog::~CLog()
{
  this->access.enter();
  
  this->log_file.close();
  this->enabled=false;
  this->access.exit();
}
