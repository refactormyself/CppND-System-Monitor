#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
  
  float utime{}, stime{}, cutime{}, cstime{}, start_time{};
  string token{"0"};
  std::ifstream stream(LinuxParser::kProcDirectory 
                          + '/'+ to_string(pid) + LinuxParser::kStatFilename);

  if(stream.is_open()) {
    
    string line{};
    std::getline(stream, line);    
    std::istringstream iss(line);
    
    int counter = 0; 
    while (iss >> token) 
    { 
      if (counter == 13)
      {
        utime = std::stol(token);
      }
      else if (counter == 14)
      {
        stime = std::stol(token);
      }
      else if (counter == 15)
      {
        cutime = std::stol(token);
      }
      else if (counter == 16)
      {
        cstime = std::stol(token);
      }
      else if (counter == 21)
      {
        start_time = std::stol(token);
        break;
      }
      ++counter;
    }    
  }

  long hertz = sysconf(_SC_CLK_TCK);
  long uptime = LinuxParser::UpTime();
  
  auto total_time = utime + stime;
  total_time += cutime + cstime;
  auto seconds =  uptime - (start_time / hertz);
  auto cpu_usage = ((total_time / hertz) / seconds);

  cpuUtilization = cpu_usage;
  
  return cpuUtilization;
}

// Return the command that generated this process
string Process::Command() { 
  return command; 
}

// Return this process's memory utilization
string Process::Ram() { 
  return to_string(ram);   
}

// Return the user (name) that generated this process
string Process::User() { return user; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return 12356; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const  { 
   //return cpuUtilization < a.cpuUtilization;
   return ram > a.ram;
   //return std::stoi(ram) > std::stoi(a.ram);
 } //by most active

void Process::Update(){
  user = LinuxParser::User(pid);
  upTime = LinuxParser::UpTime(pid);
  command = LinuxParser::Command(pid);
  ram = LinuxParser::Ram(pid);

}

