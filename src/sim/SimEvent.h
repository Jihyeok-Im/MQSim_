#ifndef SIM_EVENT_H_
#define SIM_EVENT_H_

#include "SimDef.h"
#include "SimObject.h"

static u_int64_t secondary_key = 0;

namespace sim_engine {
class SimObject;
class SimEvent {
 public:
  SimEvent(SimTime fire_time, SimObject* target_sim_object,
           void* parameters = NULL, int type = 0)
      : fire_time_(fire_time),
        target_sim_object_(target_sim_object),
        parameters_(parameters),
        type_(type),
        secondary_key_(++secondary_key) {}  // to manage same fire_time
  ~SimEvent(){};
  SimTime get_firetime() { return fire_time_; }
  SimObject* get_target_object() { return target_sim_object_; }
  void* get_parameters() { return parameters_; }
  int get_type() { return type_; }
  unsigned int get_secondary_key() { return secondary_key_; }

 private:
  SimTime fire_time_;
  SimObject* target_sim_object_;
  void* parameters_ = NULL;
  int type_;
  unsigned int secondary_key_;
};
}  // namespace sim_engine

#endif