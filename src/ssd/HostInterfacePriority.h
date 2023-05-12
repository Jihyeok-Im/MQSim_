#ifndef HOST_INTERFACE_PRIORITIY_H_
#define HOST_INTERFACE_PRIORITIY_H_

#include <stdexcept>
#include <string>

class IOFlowPriorityClass {
 public:
  enum Priority {
    kUrgent = 0,
    kHigh = 1,
    kMedium = 2,
    kLow = 3,
    kUndefined = 10000
  };

  static const int NUMBER_OF_PRIORITY_LEVELS = 4;

  static int get_scheduling_weight(Priority priority) {
    switch (priority) {
      case kUrgent:
        return INT32_MAX;
      case kHigh:
        return 4;
      case kMedium:
        return 2;
      case kLow:
        return 1;
      default:
        return 0;
    }
  }

  static std::string ToString(Priority priority) {
    switch (priority) {
      case kUrgent:
        return "URGENT";
      case kHigh:
        return "HIGH";
      case kMedium:
        return "MEDIUM";
      case kLow:
        return "LOW";
      default:
        return "";
    }
  }

  static std::string ToString(unsigned int priority) {
    return ToString(static_cast<IOFlowPriorityClass::Priority>(priority));
  }

  static Priority ToPriority(int priorityInt) {
    switch (priorityInt) {
      case 0:
        return kUrgent;
      case 1:
        return kHigh;
      case 2:
        return kMedium;
      case 3:
        return kLow;
      default:
        throw std::invalid_argument("Priority level integer value");
    }
  }
};

#endif  // !HOST_INTERFACE_PRIORITY_H_