#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "../include/process.h"
#include "../include/linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return pid_; }

// Set Cpu utilization
void Process::CpuUtilization(std::vector<std::string> stats_string, long system_uptime) {
  unsigned long long int total_time = 0;
  float seconds = 0;
  float hertz = sysconf(_SC_CLK_TCK);
  string stats_string_concat = "";
  PidStat stats;

  // Calculate cpu usage per
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  if (stats_string.size() >= 22) {
    stats.utime = std::stoul(stats_string[LinuxParser::ProcessStats::kUtime]);
    stats.stime = std::stoul(stats_string[LinuxParser::ProcessStats::kStime]);
    stats.cutime = std::stoul(stats_string[LinuxParser::ProcessStats::kCutime]);
    stats.cstime = std::stoul(stats_string[LinuxParser::ProcessStats::kCstime]);
    stats.starttime = std::stoul(stats_string[LinuxParser::ProcessStats::kStarttime]);
  }
  total_time = stats.utime + stats.stime;
  // Only include if child processes should be counted too:
  // total_time = total_time + stats.cutime + stats.cstime;
  seconds = system_uptime - (stats.starttime / hertz);
  cpu_ = (total_time / hertz) / seconds;
}

// Return this process's CPU utilization in percent
float Process::CpuUtilization() { return cpu_; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// Return the command that generated this process
string Process::Command() { return command_; }

// Return the user (name) that generated this process
string Process::User() { return username_; }

// Return this process's memory utilization
string Process::Ram() { return to_string(static_cast<int>(ram_)); }

// Set user id and username
void Process::User(vector<string> statuses) {
  string uid = "";
  string key = "";

  // Find Uid and set class data
  for ( auto status : statuses ) {
    std::istringstream linestream(status);
    while (linestream >> key >> uid) {
      if (key == LinuxParser::kFilterUID)
        uid_ = uid;
    }
  }

  // Use uid to identify username and set class data
  username_ = LinuxParser::ReadUser(uid_);
}

// Set Ram
void Process::Ram(vector<string> statuses) {
  int ram = 0;
  string key = "";
  float kb_per_mb = 1024.0;

  for ( auto status : statuses ) {
    std::istringstream linestream(status);
    while (linestream >> key >> ram) {
      if (key == LinuxParser::kFilterProcMem)
        ram_ = ram / kb_per_mb;
    }
  }
}

// Set command
void Process::Command(string commands) {
  string command = "";

  // Extract first entry and set to class data
  std::istringstream linestream(commands);
  linestream >> command;
  command_ = command;
}

// Set Uptime
void Process::UpTime(vector<string> stats) {
  int uptime_index = 13;  // Uptime index
  long process_uptime = 0;

  if (stats.size() >= 14) {
    process_uptime = std::stoul(stats[uptime_index]);
  }

  // TODO try to understand the reviewer's comment, so far it's not making sense
  uptime_ = process_uptime / sysconf(_SC_CLK_TCK);  // Convert ticks to seconds
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator>(Process const& a) const {
  return cpu_ > a.cpu_;
}