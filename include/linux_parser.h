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
const string kProcDirectory{"/proc/"};
const string kCmdlineFilename{"/cmdline"};
const string kCpuinfoFilename{"/cpuinfo"};
const string kStatusFilename{"/status"};
const string kStatFilename{"/stat"};
const string kUptimeFilename{"/uptime"};
const string kMeminfoFilename{"/meminfo"};
const string kVersionFilename{"/version"};
const string kOSPath{"/etc/os-release"};
const string kPasswordPath{"/etc/passwd"};

// Constants
const string kFilterProcesses("processes");
const string kFilterRunningProcesses("procs_running");
const string kFilterMemTotal("MemTotal");
const string kFilterMemFree("MemFree");
const string kFilterCpu("cpu");
const string kFilterUID("Uid");
      // Using VmRSS instead of VmSize because I want physical memory usage
      // VmSize can give more than physical RAM size which is misleading
      // See https://man7.org/linux/man-pages/man5/proc.5.html
const string kFilterProcMem("VmRSS");
const string kFilterkB("kB");
const string kFilterPrettyName("PRETTY_NAME");

// System
float MemoryUtilization();
long UpTime();
vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
string OperatingSystem();
string Kernel();

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

long ReadUptime();
vector<string> ReadStat();
long Jiffies(vector<string> stats);
long ActiveJiffies(vector<string> stats);
// long ActiveJiffies(int pid);
long IdleJiffies(vector<string> stats);

// Processes
string ReadCommand(int pid);
string ReadUser(std::string uid);
vector<string> ReadStat(int pid);
vector<string> ReadStatus(int pid);

};  // namespace LinuxParser

#endif  // LINUX_PARSER_H_
