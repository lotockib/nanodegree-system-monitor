#ifndef PROCESSOR_H_
#define PROCESSOR_H_

struct Jiffies {
  long idle = 0;
  long active = 0;
  long total = 0;
};

class Processor {
 public:
  Processor() {
    ReadStatus();
  }
  float Utilization();

 private:
  Jiffies current_;
  Jiffies previous_;
  void ReadStatus();
};

#endif  // PROCESSOR_H_
