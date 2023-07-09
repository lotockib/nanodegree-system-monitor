#ifndef PROCESS_H_
#define PROCESS_H_

#include <string>
#include <vector>
#include "../include/linux_parser.h"

using std::string;
using std::vector;

// Stats for the process
struct PidStat {
  unsigned long long int utime;
  unsigned long long int stime;
  unsigned long long int cutime;
  unsigned long long int cstime;
  unsigned long long int starttime;
};

/*
Contains data for each process.
*/
class Process {
 public:
  Process(int pid) : pid_(pid) {
    // Read each file only one time
    long system_uptime = LinuxParser::ReadUptime();
    vector<string> stats = LinuxParser::ReadStat(pid_);
    vector<string> statuses = LinuxParser::ReadStatus(pid_);
    string commands = LinuxParser::ReadCommand(pid_);

    // Set local member data using file strings
    CpuUtilization(stats, system_uptime);
    UpTime(stats, system_uptime);
    Ram(statuses);
    User(statuses);
    Command(commands);
  }

  // Getters
  int Pid();
  float CpuUtilization();
  long int UpTime();
  string Ram();
  string User();
  string Command();
  void Pid(int p) { pid_ = p; }
  bool operator>(Process const& a) const;

 private:
  // Setters
  void CpuUtilization(vector<string> stats, long system_uptime);
  void UpTime(vector<string> stats, long system_uptime);
  void Ram(vector<string> statuses);
  void User(vector<string> statuses);
  void Command(string commands);

  // Data
  int pid_{0};
  float cpu_{0};
  long int uptime_{0};
  float ram_{0};
  string uid_{};
  string username_{};
  string command_{};
};

#endif  // PROCESS_H_
