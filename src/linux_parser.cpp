#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
namespace fs = std::filesystem;

// An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}


// TODO BONUS: Update this to use std::filesystem
// Gets the list of current PIDs on the system
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


/*

bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   std::strtol(s.c_str(), &p, 10);

   return (*p == 0);
}

vector<int> LinuxParser::Pids() {
  vector<int> pids; int count = 0;
  for (const auto &content : fs::directory_iterator(kProcDirectory))
  {
    auto filename = content.path().filename().c_str();
    if (fs::is_directory(content.status()) && isInteger(filename))
    {
      int pid = std::stoi(filename);
      pids.push_back(pid);
    }

  }
  return pids;
}
*/



// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 

  float memTotal{}, memFree{};//, buffers{};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  int max_line_cnt = 4;

  if (stream.is_open())
  {
    for (std::string line; std::getline(stream, line) && max_line_cnt > 0; --max_line_cnt ) {
      
    std::string label{};
    
    vector<string> tokens;
    string token; 
    std::istringstream iss(line); 
    
    while (iss >> token) 
    { tokens.push_back(token); }
            
    if (tokens[0] == "MemTotal:")
    {
        memTotal = std::stof(tokens[1].c_str());
    }
    else if (tokens[0] == "MemFree:")
    {
        memFree = std::stof(tokens[1].c_str());
    }
    //else if (tokens[0] == "Buffers:")
    //{
    //    buffers = std::stof(tokens[1].c_str());
    //}
   // else
   // {
   //     std::cout<<"Found : "<< tokens[0] <<'\n'<<"ERROR PARSING MEMORY...\n";
   //     break;
   // }
  }

  }
  
  // return 1.0 - (memFree / (memTotal - buffers));
  return(memTotal - memFree) / memTotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() { 

  string uptime{"0"};
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  
  return std::stol(uptime); 
  }

// TODO Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return UpTime() * sysconf(_SC_CLK_TCK);  
}

// TODO Read and return the number of active jiffies for a PID

long LinuxParser::ActiveJiffies(int pid) { 

  std::ifstream stream(kProcDirectory + '/'+ to_string(pid) + kStatFilename);
  long total{};
  
  if(stream.is_open()) {

    string line{};
    std::getline(stream, line);
    
    vector<string> tokens;
    string token; 
    std::istringstream iss(line);
    
    int counter = 0; 
    while (iss >> token) 
    { 
      if (counter >12 && counter < 17)
      {
        tokens.push_back(token);
      }
      ++counter;
    }
    
    for (const auto &token : tokens)
    {
      total += std::stol(token);
    }    
  }
  return total; 
}

// TODO Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 

  std::ifstream stream(kProcDirectory + kStatFilename);
  long total{};
  
  if(stream.is_open()) {

    string line{};
    std::getline(stream, line);
    
    vector<string> tokens;
    string token; 
    std::istringstream iss(line);
    
    bool skip = false; 
    while (iss >> token) 
    { 
      if (skip)
      {
        tokens.push_back(token);
      }
      skip = true;
    }
    
    for (const auto &token : tokens)
    {
      total += std::stol(token);
    }    
  }
  return total; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 

  std::ifstream stream(kProcDirectory + kStatFilename);
  long total{};
  vector<string> tokens;
    
  if(stream.is_open()) {

    string line{};
    std::getline(stream, line);
    
    string token; 
    std::istringstream iss(line);
    
    bool skip = false; 
    while (iss >> token) 
    { 
      if (skip)
      {
        tokens.push_back(token);
      }
      skip = true;
    }         
  }
  if (tokens.size()>4)
  {
    total = std::stol(tokens[3]) + std::stol(tokens[4]);
  }
  
  return total; 
}

// DONE: Read and return CPU utilization
// vector<string> LinuxParser::CpuUtilization() { return {}; }
float LinuxParser::CpuUtilization(){

  std::ifstream stream(kProcDirectory + kStatFilename);
  float total{}, active{};
  
  if(stream.is_open()) {

    string line{};
    std::getline(stream, line);
    
    vector<float> tokens;
    float token; 
    std::istringstream iss(line);
    string label{};
    iss >> label;

    if (label == "cpu")
    {
      while (iss >> token) 
      { 
        tokens.push_back(token);
      }
    }
    
    for (float token : tokens){
	total+=token;
    }

    active = total - (tokens[3] + tokens[4]); 
  }
  return active / total; 

}

long ProcStat_(string label) { 

  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  string token("0");
        
  if(stream.is_open()) {

    string line{};
    while (std::getline(stream, line))
    {
      std::istringstream iss(line);
      iss >> token;

      if (token == label)
      {
        iss >> token;
        break;
      }else{
        token = "0";
      }
    }
  }
  return std::stol(token);
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { return ProcStat_("processes");}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { return ProcStat_("procs_running");}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 

  string line{};
  auto path = kProcDirectory + '/'+ to_string(pid) + kCmdlineFilename;
  std::ifstream stream(path);

  if (stream.is_open()) {
    std::getline(stream, line);    
  }
  
  return line; 
}

// Read and return the memory used by a process
long LinuxParser::Ram(int pid[[maybe_unused]]) {  

    auto path = kProcDirectory + '/'+ to_string(pid) + kStatusFilename;
    std::ifstream stream(path);
    
    if (stream.is_open())
    {
      for (std::string line; std::getline(stream, line); ) {
      
        std::string label{};

        vector<string> tokens;
        string token; 
        std::istringstream iss(line); 
        while (iss >> token) 
        { tokens.push_back(token); }
                
        if (tokens[0] == "VmSize:")
        {
            //convert to MB from KB
            //auto ret = to_string(std::stoi(tokens[1]) / 1024);
            //return std::all_of(ret.begin(), ret.end(), isspace)? "0" : ret;
	    
	    return long(std::stoi(tokens[1]) / 1024);
        }
      }
    }    
    return 0; 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {  

    auto path = kProcDirectory + '/'+ to_string(pid) + kStatusFilename;
    std::ifstream stream(path);
    
    if (stream.is_open())
    {
      for (std::string line; std::getline(stream, line); ) {
      
        std::string label{};
        
        vector<string> tokens;
        string token; 
        std::istringstream iss(line); 
        while (iss >> token) 
        { tokens.push_back(token); }
                
        if (tokens[0] == "Uid:")
        {
            return tokens[1];
        }
      }
    }    
    return ""; 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {  

  string name{};
  string uid = Uid(pid);
  std::ifstream stream(kPasswordPath);

  //saheed:x:1000:1000:saheed:/home/saheed:/bin/bash
  if (stream.is_open())
  {
    for (std::string line; std::getline(stream, line); ) {
      
      //string nm = line.substr(0, line.find(":x:"));
      //line.erase(0, line.find(":x:") + nm.length());
      //string id = line.substr(0, line.find(":"));
      
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream iss(line);

      string token{}, x{};
      while(iss >> name >> x >> token)
      {      	
        if (token == uid)
        {
          return name;
        }//else{return "@"+id;}      
      }          
    }
  }    
  return uid;
}


// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 

  long start_time{};
  string token{"0"};
  std::ifstream stream(kProcDirectory + '/'+ to_string(pid) + kStatFilename);

  if(stream.is_open()) {
    
    string line{};
    std::getline(stream, line);    
    std::istringstream iss(line);
    
    int counter = 0; 
    while (iss >> token) 
    { 
      if (counter == 21)
      {
        start_time = std::stol(token);
        return start_time / sysconf(_SC_CLK_TCK);
      }
      ++counter;
    }    
  }

  return 0;   
}
