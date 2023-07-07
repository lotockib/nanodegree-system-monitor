#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "../include/linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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
        if (key == kFilterPrettyName) {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value;
  string value_units;
  float mem_total = 0;
  float mem_free = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> value_units) {
        if (key == kFilterMemTotal && value_units == kFilterkB)
          mem_total = value;
        else if (key == kFilterMemFree && value_units == kFilterkB)
          mem_free = value;
      }
    }
  }
  return (mem_total - mem_free) / mem_total;
  }

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line = "";
  double uptime = 0;
  double uptime_incl_suspend = 0;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> uptime_incl_suspend;
  }
  return static_cast<long int>(uptime);
  }

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies(vector<string> stats) {
  return
    std::stoul(stats[CPUStates::kIdle_]) +
    std::stoul(stats[CPUStates::kIOwait_]) +
    std::stoul(stats[CPUStates::kUser_]) +
    std::stoul(stats[CPUStates::kNice_]) +
    std::stoul(stats[CPUStates::kSystem_]) +
    std::stoul(stats[CPUStates::kIRQ_]) +
    std::stoul(stats[CPUStates::kSoftIRQ_]) +
    std::stoul(stats[CPUStates::kSteal_]);
  }

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies(vector<string> stats) {
  return
    std::stoul(stats[CPUStates::kUser_]) +
    std::stoul(stats[CPUStates::kNice_]) +
    std::stoul(stats[CPUStates::kSystem_]) +
    std::stoul(stats[CPUStates::kIRQ_]) +
    std::stoul(stats[CPUStates::kSoftIRQ_]) +
    std::stoul(stats[CPUStates::kSteal_]);
  }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies(vector<string> stats) {
  return
    std::stoul(stats[CPUStates::kIdle_]) +
    std::stoul(stats[CPUStates::kIOwait_]);
  }

// Read uptime
long LinuxParser::ReadUptime() {
  string line = "";
  long uptime = 0;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  std::getline(filestream, line);
  std::istringstream linestream(line);
  linestream >> uptime;  // System uptime in seconds
  return uptime;
}


// Read stat file and return string
vector<string> LinuxParser::ReadStat() {
  string line = "";
  string value = "";
  string key = "cpu";
  vector<string> stats = {};

  // Access file, read, and store stat data
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "cpu") {
        while (linestream >> value)
          stats.push_back(value);
        return stats;
      }
    }
  }
  return stats;
  }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == kFilterProcesses) {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == kFilterRunningProcesses) {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the command associated with a process
string LinuxParser::ReadCommand(int pid) {
  string line = {};
  string cmd = {};

  // Get UID
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::replace(line.begin(), line.end(), '\000', ' ');
    std::istringstream linestream(line);
    linestream >> cmd;
  }

  return cmd;
}

// Read status file and return strings of interest
vector<string> LinuxParser::ReadStatus(int pid) {
  string line = {};
  string key = {};
  vector<string> statuses = {};

  // Read status and build vector of lines of interest
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line) && statuses.size() < 2) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key) {
        if ( key == kFilterProcMem || key == kFilterUID )  // We only need to process these lines
          statuses.push_back(line);
      }
    }
  }

  return statuses;
}

// Read and return the user associated with a process
string LinuxParser::ReadUser(string uid) {
  string line;
  string username = "";
  string pass_col1 = "";
  string pass_uid = "";

  // Get Username
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line) && pass_uid != uid) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> username >> pass_col1 >> pass_uid) {
        if (pass_uid == uid)
          return username;
      }
    }
  }

  return username;
}

// Read stat file and return string
vector<string> LinuxParser::ReadStat(int pid) {
  string line = "";
  string value = "";
  vector<string> stats = {};

  // Access file, read, and store stat data
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        stats.push_back(value);
        }
      }
    }
  return stats;
  }
