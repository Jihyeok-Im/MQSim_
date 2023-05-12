#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

#include <algorithm>
#include <functional>
#include <queue>
#include <vector>

#include "SimEvent.h"

namespace sim_engine {
class EventComparator {
 public:
  bool operator()(sim_engine::SimEvent* Lhs, sim_engine::SimEvent* Rhs) {
    if ((*Lhs).get_firetime() != (*Rhs).get_firetime()) {
      return (*Lhs).get_firetime() > (*Rhs).get_firetime();
    }
    return (*Lhs).get_secondary_key() > (*Rhs).get_secondary_key();
  }
};

template <typename Elem = sim_engine::SimEvent*,
          typename Compare = EventComparator,
          typename Queue = std::vector<sim_engine::SimEvent*>>
class EventQueue {
 public:
  EventQueue() {}
  void push(const Elem& event, const Compare& comp = Compare()) {
    queue_.push_back(event);
    std::push_heap(queue_.begin(), queue_.end(), comp);
  }

  void pop() {
    std::pop_heap(queue_.begin(), queue_.end(), compare_);
    queue_.pop_back();
  }

  const Elem& top() const { return queue_.front(); }

  bool empty() const { return queue_.empty(); }

 private:
  Queue queue_;
  Compare compare_;
};
using EventQueue_ =
    EventQueue<sim_engine::SimEvent*, sim_engine::EventComparator,
               std::vector<sim_engine::SimEvent*>>;
}  // namespace sim_engine

#endif