#include <string>
#include <vector>
#include "../include/processor.h"
#include "../include/linux_parser.h"

using std::string;
using std::vector;

// Read from /proc/stat and store in private member data
void Processor::ReadStatus() {

  // Get stats from file
  vector<string> stats = LinuxParser::ReadStat();

  // Before processing new data, move current to previous
  previous_ = current_;

  // Get active and idle jiffies
  current_.idle = LinuxParser::IdleJiffies(stats);
  current_.active = LinuxParser::ActiveJiffies(stats);
  current_.total = LinuxParser::Jiffies(stats);
}

// Return the aggregate CPU utilization
float Processor::Utilization() {
  long totald = 0;
  long idled = 0;

  // Get latest CPU stats
  ReadStatus();

  // Calculate CPU usage per
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  totald = current_.total - previous_.total;
  idled = current_.idle - previous_.idle;

  if ( totald == 0 )
    return 0.0;
  return (totald - idled) / static_cast<float>(totald);
}
