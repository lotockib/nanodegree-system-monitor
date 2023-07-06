#ifndef LINUX_PARSER_H_
#define LINUX_PARSER_H_

#include <fstream>
#include <regex>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_ = 1,
  kSystem_ = 2,
  kIdle_ = 3,
  kIOwait_ = 4,
  kIRQ_ = 5,
  kSoftIRQ_ = 6,
  kSteal_ = 7,
  kGuest_ = 8,
  kGuestNice_ = 9
};

// Process
enum ProcessStats {
  kUtime = 13,
  kStime = 14,
  kCutime = 15,
  kCstime = 16,
  kStarttime = 21
};

std::vector<std::string> ReadStat();
long Jiffies(vector<string> stats);
long ActiveJiffies(vector<string> stats);
// long ActiveJiffies(int pid);
long IdleJiffies(vector<string> stats);

// Processes
std::string ReadCommand(int pid);
std::string ReadUser(std::string uid);
std::vector<std::string> ReadStat(int pid);
std::vector<std::string> ReadStatus(int pid);

};  // namespace LinuxParser

#endif  // LINUX_PARSER_H_
