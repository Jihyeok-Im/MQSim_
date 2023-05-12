#ifndef ENGINE_H_
#define ENGINE_H_

#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>

#include "EventQueue.h"
#include "SimDef.h"
#include "SimObject.h"

namespace sim_engine {
class Engine {
 public:
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
  static Engine* Instance();
  SimTime Time();
  SimEvent* RegisterSimulatorEvent(SimTime fire_time, SimObject* target_object,
                                   void* parameters = NULL, int type = 0);
  void Reset();
  void AddObject(SimObject* obj);
  void StartSimulation();

 protected:
  Engine() { started_ = false; }
  virtual ~Engine() {}

 private:
  EventQueue_ event_queue_;
  SimTime sim_time_;
  std::unordered_map<SimObjectID, SimObject*> object_list_;
  bool started_;
  static Engine* instance_;
};
}  // namespace sim_engine

#define Simulator sim_engine::Engine::Instance()
#endif