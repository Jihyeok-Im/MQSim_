#include "Die.h"

namespace nand_flash_memory {
Die::Die(unsigned int planes_per_die, unsigned int blocks_per_plane,
         unsigned int pages_per_block, MLCTechnology mlc_technology)
    : num_plane_(planes_per_die),
      die_status_(DieStatus::kIdle),
      command_finish_event_(NULL),
      expected_finish_time_(INVALID_TIME),
      current_command_(NULL),
      stat_total_program_time_(0),
      stat_total_read_time_(0),
      stat_total_erase_time_(0),
      stat_total_transfer_time_(0),
      mlc_technology_(mlc_technology) {
  planes_ = new Plane*[planes_per_die];
  for (unsigned int i = 0; i < planes_per_die; i++) {
    planes_[i] = new Plane(blocks_per_plane, pages_per_block);
  }
}

Die::~Die() {
  for (unsigned int plane_id = 0; plane_id < num_plane_; plane_id++) {
    delete planes_[plane_id];
  }
  delete[] planes_;
}
}  // namespace nand_flash_memory