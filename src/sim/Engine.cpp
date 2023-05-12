#include "Engine.h"

#include <cstdio>
#include <stdexcept>

#include "../utils/Logical_Address_Partitioning_Unit.h"

namespace sim_engine {
Engine* Engine::instance_ = NULL;

Engine* Engine::Instance() {
  if (instance_ == 0) {
    instance_ = new Engine;
  }
  return instance_;
}

void Engine::Reset() {
  object_list_.clear();
  sim_time_ = 0;
  started_ = false;
  secondary_key = 0;
  Utils::Logical_Address_Partitioning_Unit::Reset();
}

void Engine::AddObject(SimObject* obj) {
  if (object_list_.find(obj->get_id()) != object_list_.end()) {
    throw std::invalid_argument("Duplicate object key: " + obj->get_id());
  }
  object_list_.insert(std::pair<SimObjectID, SimObject*>(obj->get_id(), obj));
}

void Engine::StartSimulation() {
  started_ = true;

  for (std::unordered_map<SimObjectID, SimObject*>::iterator obj =
           object_list_.begin();
       obj != object_list_.end(); ++obj) {
    if (!obj->second->IsTriggerSetUp()) {
      obj->second->SetupTriggers();
    }
  }

  for (std::unordered_map<SimObjectID, SimObject*>::iterator obj =
           object_list_.begin();
       obj != object_list_.end(); ++obj) {
    obj->second->ValidateSimulationConfig();
  }

  for (std::unordered_map<SimObjectID, SimObject*>::iterator obj =
           object_list_.begin();
       obj != object_list_.end(); ++obj) {
    obj->second->StartSimulation();
  }

  while (!event_queue_.empty()) {
    auto* event = event_queue_.top();
    sim_time_ = event->get_firetime();

    event->get_target_object()->ExecuteSimulatorEvent(event);
    SimEvent* consumed_event = event;

    delete consumed_event;
    event_queue_.pop();
  }
}

SimTime Engine::Time() { return sim_time_; }
SimEvent* Engine::RegisterSimulatorEvent(SimTime fire_time,
                                         SimObject* target_object,
                                         void* parameters, int type) {
  auto event = new SimEvent(fire_time, target_object, parameters, type);
  if (event->get_firetime() < Simulator->Time()) {
    PRINT_ERROR("Illegal request to register a simulation event before Now!")
  }
  event_queue_.push(event);
}

}  // namespace sim_engine