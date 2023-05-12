#ifndef FLASH_CHIP_H_
#define FLASH_CHIP_H_

#include <stdexcept>
#include <vector>

#include "../sim/Engine.h"
#include "../sim/SimDef.h"
#include "../sim/SimEvent.h"
#include "../sim/SimReporter.h"
#include "Die.h"
#include "NANDFlashCommand.h"
#include "NANDFlashDef.h"

namespace nand_flash_memory {
class Package : public sim_engine::SimObject {
  enum class Status { kBusy, kIdle };
  enum class PackageSimulatorEventType { kCommandFinished };

 public:
  Package(const SimObjectID& kID, ChannelID channel_id, PackageID package_id,
          MLCTechnology flash_technology, unsigned int num_die,
          unsigned int plane_no_per_die, unsigned int block_no_per_plane,
          unsigned int page_no_per_block, SimTime** read_latency,
          SimTime* program_latency, SimTime* erase_latency);
  ~Package();

  void StartCommandTransfer() {
    this->last_transfer_start_time_ = Simulator->Time();
  }
  void StartCommandDataInTransfer() {
    this->last_transfer_start_time_ = Simulator->Time();
  }
  void StartDataOutTransfer() {
    this->last_transfer_start_time_ = Simulator->Time();
  }
  void EndCommandTransfer(NANDFlashCommand* command) {
    this->stat_total_transfer_time_ +=
        (Simulator->Time() - this->last_transfer_start_time_);
    if (this->num_idle_dies_ != num_die_) {
      stat_total_overlapped_transfer_execution_time_ +=
          (Simulator->Time() - last_transfer_start_time_);
    }
    this->dies_[PPA_TO_DIE_ID(command->get_address()[0])]
        ->stat_total_transfer_time_ +=
        (Simulator->Time() - last_transfer_start_time_);
    StartCommandExecution(command);
    this->last_transfer_start_time_ = INVALID_TIME;
  }
  void EndCommandDataInTransfer(NANDFlashCommand* command) {
    this->stat_total_transfer_time_ +=
        (Simulator->Time() - this->last_transfer_start_time_);
    if (this->num_idle_dies_ != num_die_) {
      stat_total_overlapped_transfer_execution_time_ +=
          (Simulator->Time() - last_transfer_start_time_);
    }
    this->dies_[PPA_TO_DIE_ID(command->get_address()[0])]
        ->stat_total_transfer_time_ +=
        (Simulator->Time() - last_transfer_start_time_);
    StartCommandExecution(command);
    this->last_transfer_start_time_ = INVALID_TIME;
  }
  void EndDataOutTransfer(NANDFlashCommand* command) {
    this->stat_total_transfer_time_ +=
        (Simulator->Time() - this->last_transfer_start_time_);
    if (this->num_idle_dies_ != num_die_) {
      stat_total_overlapped_transfer_execution_time_ +=
          (Simulator->Time() - last_transfer_start_time_);
    }
    this->dies_[PPA_TO_DIE_ID(command->get_address()[0])]
        ->stat_total_transfer_time_ +=
        (Simulator->Time() - last_transfer_start_time_);
    this->last_transfer_start_time_ = INVALID_TIME;
  }

  void StartSimulation();
  void ValidateSimulationConfig();
  void SetupTriggers();
  void ExecuteSimulatorEvent(sim_engine::SimEvent*);

  typedef void (*ChipReadySignalHandlerType)(Package* target_package,
                                             NANDFlashCommand* command);
  void ConnectToChipReadySignal(ChipReadySignalHandlerType);

  PageType get_page_type(MLCTechnology mlc_technology, PageID page_id);
  // SimTime GetCommandExecutionLatency(NANDFlashCommandCode command_code,
  //                                    PageID page_id);
  SimTime get_command_execution_latency(NANDFlashCommandCode command_code,
                                        PageID page_id,
                                        MLCTechnology mlc_technology);

  void Report_results_in_XML(std::string name_prefix,
                             Utils::XmlWriter& xmlwriter);

 private:
  ChannelID channel_id_;
  PackageID package_id_;
  MLCTechnology mlc_technology_;
  Status status_;
  unsigned int num_idle_dies_;
  Die** dies_;
  unsigned int num_die_;
  unsigned int planes_per_die_;
  unsigned int blocks_per_plane_;
  unsigned int pages_per_block_;
  SimTime **read_latency_, *program_latency_, *erase_latency_;
  SimTime last_transfer_start_time_, execution_start_time_,
      expected_finish_time_;

  unsigned long stat_read_count_, stat_program_count_, stat_erase_count_;
  SimTime stat_total_execution_time_, stat_total_transfer_time_,
      stat_total_overlapped_transfer_execution_time_;
  void StartCommandExecution(NANDFlashCommand* command);
  void FinishCommandExecution(NANDFlashCommand* command);
  void BroadcastReadySignal(NANDFlashCommand* command);
  SimTime get_read_latency(PageType page_type);
  SimTime get_program_latency(MLCTechnology mlc_technology);
  SimTime get_erase_latency(MLCTechnology mlc_technology);
  std::vector<ChipReadySignalHandlerType> connected_ready_handlers_;
};
}  // namespace nand_flash_memory

#endif