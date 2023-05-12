#ifndef DIE_H_
#define DIE_H_

#include "../sim/SimEvent.h"
#include "NANDFlashDef.h"
#include "Plane.h"

namespace nand_flash_memory {
enum class DieStatus { kBusy, kIdle };
class Die {
 public:
  Die(unsigned int planes_per_die, unsigned int blocks_per_plane,
      unsigned int pages_per_block, MLCTechnology mlc_technology);
  ~Die();

  SimTime stat_total_program_time_, stat_total_read_time_,
      stat_total_erase_time_, stat_total_transfer_time_;

  void set_command_finish_event(sim_engine::SimEvent* event) {
    command_finish_event_ = event;
  }
  void set_die_status(DieStatus die_status) { die_status_ = die_status; }
  void set_expected_finish_time(SimTime expected_finish_time) {
    expected_finish_time_ = expected_finish_time;
  }
  SimTime get_expected_finish_time() { return expected_finish_time_; }
  void set_current_command(NANDFlashCommand* current_command) {
    current_command_ = current_command;
  }
  Plane* get_plane(PlaneID plane_id) { return planes_[plane_id]; }
  Plane** get_planes() { return planes_; }  // need for validation
  MLCTechnology get_flash_technology() { return mlc_technology_; }

 private:
  unsigned int num_plane_;
  Plane** planes_;
  sim_engine::SimEvent* command_finish_event_;
  DieStatus die_status_;
  SimTime expected_finish_time_;
  NANDFlashCommand* current_command_;
  MLCTechnology mlc_technology_;
};
}  // namespace nand_flash_memory

#endif