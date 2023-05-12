#ifndef SIM_OBJECT_H_
#define SIM_OBJECT_H_

#include <string>

#include "SimEvent.h"

namespace sim_engine {
class SimEvent;
class SimObject {
 public:
  SimObject(const SimObjectID &kID) {
    id_ = kID;
    trigger_set_up_ = false;
  }

  SimObjectID get_id() { return this->id_; };
  bool IsTriggerSetUp() { return trigger_set_up_; }

  // The Start function is invoked at the start phase of simulation to perform
  // initialization
  virtual void StartSimulation() = 0;  // Host system, IO Flow에서만 implemented

  // The Validate_simulation_config function is invoked to check if the objected
  // is correctly configured or not.
  virtual void ValidateSimulationConfig() = 0;

  // The object connects its internal functions to the outside triggering events
  // from other objects
  virtual void SetupTriggers() { trigger_set_up_ = true; }
  virtual void ExecuteSimulatorEvent(SimEvent *) = 0;

 private:
  SimObjectID id_;
  bool trigger_set_up_;
};
}  // namespace sim_engine

#endif