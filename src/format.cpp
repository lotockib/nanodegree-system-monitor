#include <string>
#include <cmath>

#include "../include/format.h"

using std::string;

// Format time into HH:MM:SS
string Format::ElapsedTime(long seconds) {
  string hrs;
  string min;
  string sec;

  // Lambda: if value is < 10, prepend 0
  auto format_to_string =
  [&] (long time) {
      if (time < 10)  // Must always have 2 digits
        return "0" + std::to_string(time);
      return std::to_string(time);
    };

  // Calculate time chunks and convert to string
  hrs = format_to_string(seconds / 3600) + ":";
  min = format_to_string((seconds % 3600) / 60) + ":";
  sec = format_to_string(((seconds % 3600) % 60) % 60);

  return hrs + min + sec;
}

// Format to string with 3 character width
string Format::MinimumWidth(int value) {
  string result = "";

  if (value < 10) {
    return " " + std::to_string(value);
  }

  if (value < 100) {
    return "  " + std::to_string(value);
  }

  return std::to_string(value);
}
