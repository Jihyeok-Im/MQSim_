#include "Package.h"

#include "../sim/Engine.h"
#include "../sim/SimDef.h"

namespace nand_flash_memory {
Package::Package(const SimObjectID& kID, ChannelID channel_id,
                 PackageID package_id, MLCTechnology mlc_technology,
                 unsigned int num_die, unsigned int planes_per_die,
                 unsigned int blocks_per_plane, unsigned int pages_per_block,
                 SimTime** read_latency, SimTime* program_latency,
                 SimTime* erase_latency)
    : SimObject(kID),
      channel_id_(channel_id),
      package_id_(package_id),
      mlc_technology_(mlc_technology),
      status_(Status::kIdle),
      num_die_(num_die),
      planes_per_die_(planes_per_die),
      blocks_per_plane_(blocks_per_plane),
      pages_per_block_(pages_per_block),
      last_transfer_start_time_(INVALID_TIME),
      execution_start_time_(INVALID_TIME),
      expected_finish_time_(INVALID_TIME),
      stat_read_count_(0),
      stat_program_count_(0),
      stat_erase_count_(0),
      stat_total_execution_time_(0),
      stat_total_transfer_time_(0),
      stat_total_overlapped_transfer_execution_time_(0) {
  int bits_per_cell = static_cast<int>(mlc_technology);
  read_latency_ = new SimTime*[bits_per_cell];
  program_latency = new SimTime[bits_per_cell];
  erase_latency = new SimTime[bits_per_cell];
  for (int i = 0; i < bits_per_cell; i++) {
    read_latency_[i] = new SimTime[i + 1];
    for (int j = 0; j < i + 1; j++) {
      read_latency_[i][j] = read_latency[i][j];
    }
    program_latency_[i] = program_latency[i];
    erase_latency_[i] = erase_latency[i];
  }
  num_idle_dies_ = num_die;
  dies_ = new Die*[num_die];
  for (unsigned int die_id = 0; die_id < num_die; die_id++) {
    dies_[die_id] = new Die(planes_per_die, blocks_per_plane, pages_per_block,
                            mlc_technology);
  }
}

Package::~Package() {
  for (unsigned int die_id = 0; die_id < num_die_; die_id++) {
    delete dies_[die_id];
  }
  delete dies_;
  for (unsigned int i = 0; i < static_cast<int>(mlc_technology_); i++) {
    delete read_latency_[i];
  }
  delete read_latency_;
  delete[] program_latency_;
  delete[] erase_latency_;
}

void Package::ConnectToChipReadySignal(ChipReadySignalHandlerType function) {
  connected_ready_handlers_.push_back(function);
}

void Package::StartSimulation() {}

void Package::ValidateSimulationConfig() {
  if (dies_ == NULL || num_die_ == 0) {
    PRINT_ERROR("Flash package " << get_id() << ": has no dies!")
  }
  for (unsigned int i = 0; i < num_die_; i++) {
    if (dies_[i]->get_planes() == NULL) {
      PRINT_ERROR("Flash package" << get_id()
                                  << ": die (" + get_id() + ") has no planes!")
    }
  }
}

void Package::SetupTriggers() { sim_engine::SimObject::SetupTriggers(); }

void Package::ExecuteSimulatorEvent(sim_engine::SimEvent* event) {
  PackageSimulatorEventType event_type =
      (PackageSimulatorEventType)event->get_type();
  NANDFlashCommand* command = (NANDFlashCommand*)event->get_parameters();
  switch (event_type) {
    case PackageSimulatorEventType::kCommandFinished:
      FinishCommandExecution(command);
      break;
  }
}

// SimTime Package::GetCommandExecutionLatency(NANDFlashCommandCode
// command_code,
//                                             PageID page_id) {
//   int latency_type = 0;
//   if (flash_technology_ == FlashTechnology::kMultiLevelCell) {
//     latency_type = page_id % 2;
//   } else if (flash_technology_ == FlashTechnology::kTripleLevelCell) {
//     latency_type =
//         (page_id <= 5) ? 0 : ((page_id <= 7) ? 1 : (((page_id - 8) >> 1) %
//         3));
//   }
//   switch (command_code) {
//     case NANDFlashCommandCode::kPageRead:
//     case NANDFlashCommandCode::kPageReadMultiplane:
//     case NANDFlashCommandCode::kPageReadCopyback:
//     case NANDFlashCommandCode::kPageReadCopybackMultiplane:
//       return read_latency_[latency_type];
//     case NANDFlashCommandCode::kPageProg:
//     case NANDFlashCommandCode::kPageProgMultiplane:
//     case NANDFlashCommandCode::kPageProgCopyback:
//     case NANDFlashCommandCode::kPageProgCopybackMultiplane:
//       return program_latency_[latency_type];

//     case NANDFlashCommandCode::kBlockErase:
//     case NANDFlashCommandCode::kBlockEraseMulitplane:
//       return erase_latency_;
//     default:
//       throw std::invalid_argument("Unsupported command for flash chip.");
//   }
// }

PageType Package::get_page_type(MLCTechnology flash_technology,
                                PageID page_id) {
  switch (flash_technology) {
    case MLCTechnology::kSingleLevelCell:
      return PageType::kSLCLSB;
    case MLCTechnology::kMultiLevelCell:
      if (page_id % 2 == 0) {
        return PageType::kMLCLSB;
      } else {
        return PageType::kMLCMSB;
      }
    case MLCTechnology::kTripleLevelCell:
      if (page_id % 3 == 0) {
        return PageType::kTLCLSB;
      } else if (page_id % 3 == 1) {
        return PageType::kTLCCSB;
      } else {
        return PageType::kTLCMSB;
      }
  }
}

SimTime Package::get_read_latency(PageType page_type) {
  switch (page_type) {
    case PageType::kSLCLSB:
      return read_latency_[0][0];
    case PageType::kMLCLSB:
      return read_latency_[1][0];
    case PageType::kMLCMSB:
      return read_latency_[1][1];
    case PageType::kTLCLSB:
      return read_latency_[2][0];
    case PageType::kTLCCSB:
      return read_latency_[2][1];
    case PageType::kTLCMSB:
      return read_latency_[2][2];
  }
}

SimTime Package::get_program_latency(MLCTechnology flash_technology) {
  switch (flash_technology) {
    case MLCTechnology::kSingleLevelCell:
      return program_latency_[0];
    case MLCTechnology::kMultiLevelCell:
      return program_latency_[1];
    case MLCTechnology::kTripleLevelCell:
      return program_latency_[2];
  }
}

SimTime Package::get_erase_latency(MLCTechnology flash_technology) {
  switch (flash_technology) {
    case MLCTechnology::kSingleLevelCell:
      return erase_latency_[0];
    case MLCTechnology::kMultiLevelCell:
      return erase_latency_[1];
    case MLCTechnology::kTripleLevelCell:
      return erase_latency_[2];
  }
}

SimTime Package::get_command_execution_latency(
    NANDFlashCommandCode command_code, PageID page_id,
    MLCTechnology flash_technology) {
  switch (command_code) {
    case NANDFlashCommandCode::kPageRead:
    case NANDFlashCommandCode::kPageReadMultiplane:
    case NANDFlashCommandCode::kPageReadCopyback:
    case NANDFlashCommandCode::kPageReadCopybackMultiplane:
      PageType page_type = get_page_type(flash_technology, page_id);
      return get_read_latency(page_type);
    case NANDFlashCommandCode::kPageProg:
    case NANDFlashCommandCode::kPageProgMultiplane:
    case NANDFlashCommandCode::kPageProgCopyback:
    case NANDFlashCommandCode::kPageProgCopybackMultiplane:
      return get_program_latency(flash_technology);
    case NANDFlashCommandCode::kBlockErase:
    case NANDFlashCommandCode::kBlockEraseMulitplane:
      return get_erase_latency(flash_technology);
    default:
      throw std::invalid_argument("Unsupported command for flash chip.");
  }
}

void Package::StartCommandExecution(NANDFlashCommand* command) {
  Die* target_die = dies_[PPA_TO_DIE_ID(command->get_address()[0])];

  target_die->set_expected_finish_time(
      Simulator->Time() +
      get_command_execution_latency(command->get_command_code(),
                                    PPA_TO_PAGE_ID(command->get_address()[0]),
                                    target_die->get_flash_technology()));
  auto target_die_expected_finish_time = target_die->get_expected_finish_time();
  target_die->set_command_finish_event(Simulator->RegisterSimulatorEvent(
      target_die_expected_finish_time, this, command,
      static_cast<int>(PackageSimulatorEventType::kCommandFinished)));
  target_die->set_current_command(command);
  target_die->set_die_status(DieStatus::kBusy);
  num_idle_dies_--;

  if (status_ == Status::kIdle) {
    execution_start_time_ = Simulator->Time();
    expected_finish_time_ = target_die_expected_finish_time;
    status_ = Status::kBusy;
  } else {
    if (expected_finish_time_ < target_die_expected_finish_time) {
      expected_finish_time_ = target_die_expected_finish_time;
    }
  }
}

void Package::FinishCommandExecution(NANDFlashCommand* command) {
  Die* target_die = dies_[PPA_TO_DIE_ID(command->get_address()[0])];
  target_die->stat_total_read_time_ += get_command_execution_latency(
      command->get_command_code(), PPA_TO_PAGE_ID(command->get_address()[0]),
      target_die->get_flash_technology());
  target_die->set_expected_finish_time(INVALID_TIME);
  target_die->set_current_command(NULL);
  target_die->set_die_status(DieStatus::kIdle);
  this->num_idle_dies_++;
  if (num_idle_dies_ == num_die_) {
    this->status_ = Status::kIdle;
    expected_finish_time_ = INVALID_TIME;
    stat_total_execution_time_ += Simulator->Time() - execution_start_time_;
    if (this->last_transfer_start_time_ != INVALID_TIME) {
      stat_total_overlapped_transfer_execution_time_ +=
          Simulator->Time() - last_transfer_start_time_;
    }
  }

  auto address = command->get_address();
  switch (command->get_command_code()) {
    case NANDFlashCommandCode::kPageRead:
    case NANDFlashCommandCode::kPageReadMultiplane:
    case NANDFlashCommandCode::kPageReadCopyback:
    case NANDFlashCommandCode::kPageReadCopybackMultiplane:
      for (unsigned int plane_count = 0; plane_count < address.size();
           plane_count++) {
        stat_read_count_++;
        target_die->get_plane(PPA_TO_PLANE_ID(address[plane_count]))
            ->get_block(PPA_TO_BLOCK_ID(address[plane_count]))
            ->get_page(PPA_TO_PAGE_ID(address[plane_count]))
            .Read(command->get_oob_data()[plane_count],
                  command->get_user_data()[plane_count]);
      }
      break;
    case NANDFlashCommandCode::kPageProg:
    case NANDFlashCommandCode::kPageProgMultiplane:
    case NANDFlashCommandCode::kPageProgCopyback:
    case NANDFlashCommandCode::kPageProgCopybackMultiplane:
      for (unsigned int plane_count = 0; plane_count < address.size();
           plane_count++) {
        stat_program_count_++;
        target_die->get_plane(PPA_TO_PLANE_ID(address[plane_count]))
            ->get_block(PPA_TO_BLOCK_ID(address[plane_count]))
            ->get_page(PPA_TO_PAGE_ID(address[plane_count]))
            .Write(command->get_oob_data()[plane_count],
                   command->get_user_data()[plane_count]);
      }
      break;
    case NANDFlashCommandCode::kBlockErase:
    case NANDFlashCommandCode::kBlockEraseMulitplane:
      for (unsigned int plane_count = 0; plane_count < address.size();
           plane_count++) {
        stat_erase_count_++;
        Block* target_block =
            target_die->get_plane(PPA_TO_PLANE_ID(address[plane_count]))
                ->get_block(PPA_TO_BLOCK_ID(address[plane_count]));
        for (unsigned int i = 0; i < pages_per_block_; i++) {
          target_block->get_page(i).set_lpa(NO_LPA);
          target_block->get_page(i).set_data(0);
        }
        break;
      }
    default:
      PRINT_ERROR("Flash chip " << get_id()
                                << ": unhandled flash command type!")
  }
  BroadcastReadySignal(command);
}

void Package::BroadcastReadySignal(NANDFlashCommand* command) {
  for (std::vector<ChipReadySignalHandlerType>::iterator it =
           connected_ready_handlers_.begin();
       it != connected_ready_handlers_.end(); it++) {
    (*it)(this, command);
  }
}

}  // namespace nand_flash_memory