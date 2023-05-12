#include "IOFlowBase.h"

#include "../sim/Engine.h"
#include "../ssd/HostInterfaceDef.h"

namespace host_components {
IOFlowBase::IOFlowBase(
    const SimObjectID& id, uint16_t flow_id,
    LogicalHostAddressType start_logical_sector_address_on_device,
    LogicalHostAddressType end_logical_sector_address_on_device,
    uint16_t io_queue_id, uint16_t nvme_submission_queue_size,
    uint16_t nvme_completion_queue_size,
    IOFlowPriorityClass::Priority priority_class, SimTime stop_time,
    double initial_occupancy_ratio, unsigned int total_requets_to_be_generated,
    PCIeRootComplex* pcie_root_complex, bool enabled_logging,
    SimTime logging_period, std::string logging_file_path)
    : SimObject(id),
      flow_id_(flow_id),
      start_logical_sector_address_on_device_(
          start_logical_sector_address_on_device),
      end_logical_sector_address_on_device_(
          end_logical_sector_address_on_device),
      io_queue_id_(io_queue_id),
      priority_class_(priority_class),
      stop_time_(stop_time),
      initial_occupancy_ratio_(initial_occupancy_ratio),
      total_requests_to_be_generated_(total_requets_to_be_generated),
      pcie_root_complex_(pcie_root_complex),
      stat_generated_request_count_(0),
      stat_generated_read_request_count_(0),
      stat_generated_write_request_count_(0),
      stat_ignored_request_count_(0),
      stat_serviced_request_count_(0),
      stat_serviced_read_request_count_(0),
      stat_serviced_write_request_count_(0),
      stat_sum_device_response_time_(0),
      stat_sum_device_response_time_read_(0),
      stat_sum_device_response_time_write_(0),
      stat_min_device_response_time_(MAXIMUM_TIME),
      stat_min_device_response_time_read_(MAXIMUM_TIME),
      stat_min_device_response_time_write_(MAXIMUM_TIME),
      stat_max_device_response_time_(0),
      stat_max_device_response_time_read_(0),
      stat_max_device_response_time_write_(0),
      stat_sum_request_delay_(0),
      stat_sum_request_delay_read_(0),
      stat_sum_request_delay_write_(0),
      stat_min_request_delay_(MAXIMUM_TIME),
      stat_min_request_delay_read_(MAXIMUM_TIME),
      stat_min_request_delay_write_(MAXIMUM_TIME),
      stat_max_request_delay_(0),
      stat_max_request_delay_read_(0),
      stat_max_request_delay_write_(0),
      stat_transferred_bytes_total_(0),
      stat_transferred_bytes_read_(0),
      stat_transferred_bytes_write_(0),
      progress_(0),
      next_progress_step_(0),
      enabled_logging_(enabled_logging),
      logging_period_(logging_period),
      logging_file_path_(logging_file_path) {
  HostIORequest* temp = NULL;

  for (uint16_t cmd_id = 0; cmd_id < (uint16_t)(0xffffffff); cmd_id++) {
    available_command_ids_.insert(cmd_id);
  }
  for (uint16_t cmd_id = 0; cmd_id < nvme_submission_queue_size_; cmd_id++) {
    request_queue_in_memory_.push_back(temp);
  }
  nvme_queue_pair_.submission_queue_size = nvme_submission_queue_size;
  nvme_queue_pair_.submission_queue_head = 0;
  nvme_queue_pair_.submission_queue_tail = 0;
  nvme_queue_pair_.completion_queue_size = nvme_completion_queue_size;
  nvme_queue_pair_.completion_queue_head = 0;
  nvme_queue_pair_.completion_queue_tail = 0;

  switch (io_queue_id) {
    case 0:
      throw std::logic_error(
          "I/O queue id 0 is reserved for NVMe admin queues and should not "
          "be used for I/O flows");
      nvme_queue_pair_.completion_queue_memory_base_address =
          kCompletionQueueRegister0;
      break;
    case 1:
      nvme_queue_pair_.submission_queue_memory_base_address =
          kSubmissionQueueMemory1;
      nvme_queue_pair_.submission_tail_register_address_on_device =
          kSubmissionQueueRegister1;
      nvme_queue_pair_.completion_queue_memory_base_address =
          kCompletionQueueMemory1;
      nvme_queue_pair_.completion_head_register_address_on_device =
          kCompletionQueueRegister1;
      break;
    case 2:
      nvme_queue_pair_.submission_queue_memory_base_address =
          kSubmissionQueueMemory2;
      nvme_queue_pair_.submission_tail_register_address_on_device =
          kSubmissionQueueRegister2;
      nvme_queue_pair_.completion_queue_memory_base_address =
          kCompletionQueueMemory2;
      nvme_queue_pair_.completion_head_register_address_on_device =
          kCompletionQueueRegister2;
      break;
    case 3:
      nvme_queue_pair_.submission_queue_memory_base_address =
          kSubmissionQueueMemory3;
      nvme_queue_pair_.submission_tail_register_address_on_device =
          kSubmissionQueueRegister3;
      nvme_queue_pair_.completion_queue_memory_base_address =
          kCompletionQueueMemory3;
      nvme_queue_pair_.completion_head_register_address_on_device =
          kCompletionQueueRegister3;
      break;
    case 4:
      nvme_queue_pair_.submission_queue_memory_base_address =
          kSubmissionQueueMemory4;
      nvme_queue_pair_.submission_tail_register_address_on_device =
          kSubmissionQueueRegister4;
      nvme_queue_pair_.completion_queue_memory_base_address =
          kCompletionQueueMemory4;
      nvme_queue_pair_.completion_head_register_address_on_device =
          kCompletionQueueRegister4;
      break;
    case 5:
      nvme_queue_pair_.submission_queue_memory_base_address =
          kSubmissionQueueMemory5;
      nvme_queue_pair_.submission_tail_register_address_on_device =
          kSubmissionQueueRegister5;
      nvme_queue_pair_.completion_queue_memory_base_address =
          kCompletionQueueMemory5;
      nvme_queue_pair_.completion_head_register_address_on_device =
          kCompletionQueueRegister5;
      break;
    case 6:
      nvme_queue_pair_.submission_queue_memory_base_address =
          kSubmissionQueueMemory6;
      nvme_queue_pair_.submission_tail_register_address_on_device =
          kSubmissionQueueRegister6;
      nvme_queue_pair_.completion_queue_memory_base_address =
          kCompletionQueueMemory6;
      nvme_queue_pair_.completion_head_register_address_on_device =
          kCompletionQueueRegister6;
      break;
    case 7:
      nvme_queue_pair_.submission_queue_memory_base_address =
          kSubmissionQueueMemory7;
      nvme_queue_pair_.submission_tail_register_address_on_device =
          kSubmissionQueueRegister7;
      nvme_queue_pair_.completion_queue_memory_base_address =
          kCompletionQueueMemory7;
      nvme_queue_pair_.completion_head_register_address_on_device =
          kCompletionQueueRegister7;
      break;
    case 8:
      nvme_queue_pair_.submission_queue_memory_base_address =
          kSubmissionQueueMemory8;
      nvme_queue_pair_.submission_tail_register_address_on_device =
          kSubmissionQueueRegister8;
      nvme_queue_pair_.completion_queue_memory_base_address =
          kCompletionQueueMemory8;
      nvme_queue_pair_.completion_head_register_address_on_device =
          kCompletionQueueRegister8;
      break;
    default:
      break;
  }
}

IOFlowBase::~IOFlowBase() {
  log_file_.close();
  for (auto& request : waiting_requests_) {
    if (request) {
      delete request;
    }
  }

  for (auto& request : nvme_software_request_queue_) {
    if (request.second) {
      delete request.second;
    }
  }
}

void IOFlowBase::StartSimulation() {
  next_logging_milestone_ = logging_period_;
  if (enabled_logging_) {
    log_file_.open(logging_file_path_, std::ofstream::out);
  }
  log_file_ << "SimulationTime(us)\t"
            << "ResponseTime(us)\t"
            << "EndToEndDelay(us)" << std::endl;
  stat_sum_device_response_time_short_term_ = 0;
  stat_serviced_request_count_short_term_ = 0;
}

void IOFlowBase::NVMeConsumeIORequest(
    CompletionQueueEntry* completion_queue_entry) {
  HostIORequest* request =
      nvme_software_request_queue_[completion_queue_entry->command_identifier];
  nvme_software_request_queue_.erase(
      completion_queue_entry->command_identifier);
  available_command_ids_.insert(completion_queue_entry->command_identifier);
  SimTime device_response_time = Simulator->Time() - request->enqueue_time_;
  SimTime request_delay = Simulator->Time() - request->arrival_time_;
  stat_serviced_request_count_++;
  stat_serviced_request_count_short_term_++;

  stat_sum_device_response_time_ += device_response_time;
  stat_sum_device_response_time_short_term_ += device_response_time;
  stat_sum_request_delay_read_ += request_delay;
  stat_sum_request_delay_short_term_ += request_delay;

  if (device_response_time > stat_max_device_response_time_) {
    stat_max_device_response_time_ = device_response_time;
  }
  if (device_response_time < stat_min_device_response_time_) {
    stat_min_device_response_time_ = device_response_time;
  }
  if (request_delay > stat_max_request_delay_) {
    stat_max_request_delay_ = request_delay;
  }
  if (request_delay < stat_min_request_delay_) {
    stat_min_request_delay_ = request_delay;
  }
  stat_transferred_bytes_total_ +=
      request->logical_block_address_count_ * SECTOR_SIZE_IN_BYTE;

  if (request->type_ == HostIORequestType::kRead) {
    stat_serviced_read_request_count_++;
    stat_sum_device_response_time_read_ += device_response_time;
    stat_sum_request_delay_read_ += request_delay;
    if (device_response_time > stat_max_device_response_time_read_) {
      stat_max_device_response_time_read_ = device_response_time;
    }
    if (device_response_time < stat_min_device_response_time_read_) {
      stat_min_device_response_time_read_ = device_response_time;
    }
    if (request_delay > stat_max_request_delay_read_) {
      stat_max_request_delay_read_ = request_delay;
    }
    if (request_delay < stat_min_request_delay_read_) {
      stat_min_request_delay_read_ = request_delay;
    }
    stat_transferred_bytes_read_ += request_delay;
  } else {
    stat_serviced_write_request_count_++;
    stat_sum_device_response_time_write_ += device_response_time;
    stat_sum_request_delay_write_ += request_delay;
    if (device_response_time > stat_max_device_response_time_write_) {
      stat_max_device_response_time_write_ = device_response_time;
    }
    if (device_response_time < stat_min_device_response_time_write_) {
      stat_min_device_response_time_write_ = device_response_time;
    }
    if (request_delay > stat_max_request_delay_write_) {
      stat_max_request_delay_write_ = request_delay;
    }
    if (request_delay < stat_min_request_delay_write_) {
      stat_min_request_delay_write_ = request_delay;
    }
    stat_transferred_bytes_write_ +=
        request->logical_block_address_count_ * SECTOR_SIZE_IN_BYTE;
  }
  delete request;

  nvme_queue_pair_.submission_queue_head =
      completion_queue_entry->sq_head_pointer;
  // assumed that reuqest is always processed correctly
  // no need to check completion_queue_entry->status_field_and_phase_tag

  // if submission queue is not full, enqueue waiting requests
  while (waiting_requests_.size() > 0) {
    if (!NVME_SQ_FULL(nvme_queue_pair_) && available_command_ids_.size() > 0) {
      HostIORequest* new_request = waiting_requests_.front();
      waiting_requests_.pop_front();
      if (nvme_software_request_queue_[*available_command_ids_.begin()] !=
          NULL) {
        PRINT_ERROR("Overwriting a waiting I/O request in the queue")
      } else {
        new_request->io_queue_info_ = *available_command_ids_.begin();
        nvme_software_request_queue_[*available_command_ids_.begin()] =
            new_request;
        available_command_ids_.erase(available_command_ids_.begin());
        request_queue_in_memory_[nvme_queue_pair_.submission_queue_tail] =
            new_request;
        NVME_UPDATE_SQ_TAIL(nvme_queue_pair_);
      }
      new_request->enqueue_time_ = Simulator->Time();
      pcie_root_complex_->WriteToDevice(
          nvme_queue_pair_.submission_tail_register_address_on_device,
          nvme_queue_pair_.submission_queue_tail);
      // based on NVMe protocol, updated tail pointer should be informed to
      // device
    } else {
      break;
    }
  }

  delete completion_queue_entry;

  if (stop_time_ > 0) {
    progress_ = int(Simulator->Time() / (double)stop_time_ * 100);
  } else {
    progress_ = int(stat_serviced_read_request_count_ /
                    (double)total_requests_to_be_generated_ * 100);
  }

  if (progress_ >= next_progress_step_) {
    std::string progress_bar;
    int bar_width = 100;
    progress_bar += "[";
    int pos = progress_;
    for (int i = 0; i < bar_width; i += 5) {
      if (i < pos) {
        progress_bar += "=";
      } else if (i == pos) {
        progress_bar += ">";
      } else {
        progress_bar += " ";
      }
    }
    progress_bar += "]";
    PRINT_MESSAGE(progress_bar << " " << progress_ << "% progress in "
                               << get_id() << std::endl)
    next_progress_step_ += 5;
  }

  if (Simulator->Time() > next_logging_milestone_) {
    log_file_ << Simulator->Time() / SIM_TIME_TO_MICROSECONDS_COEFF << "\t"
              << get_device_response_time_short_term() << "\t"
              << get_end_to_end_request_delay_short_term() << std::endl;
    stat_sum_device_response_time_short_term_ = 0;
    stat_sum_request_delay_short_term_ = 0;
    stat_serviced_request_count_short_term_ = 0;
    next_logging_milestone_ = Simulator->Time() + logging_period_;
  }
}

SubmissionQueueEntry* IOFlowBase::NVMeReadSubmissionQueueEntry(
    uint64_t address) {
  SubmissionQueueEntry* submission_queue_entry = new SubmissionQueueEntry;
  HostIORequest* request = request_queue_in_memory_[(
      uint16_t)((address -
                 nvme_queue_pair_.submission_queue_memory_base_address) /
                sizeof(SubmissionQueueEntry))];

  if (request == NULL) {
    throw std::invalid_argument(
        this->get_id() +
        ": Request to access a submission queue entry that does not exist");
  }

  submission_queue_entry->command_identifier = request->io_queue_info_;
  if (request->type_ == HostIORequestType::kRead) {
    submission_queue_entry->opcode = NVME_READ_OPCODE;
    submission_queue_entry->command_specific[0] =
        (uint32_t)request->start_logical_block_address_;
    submission_queue_entry->command_specific[1] =
        (uint32_t)(request->start_logical_block_address_ >> 32);
    submission_queue_entry->command_specific[2] =
        ((uint32_t)((uint16_t)request->logical_block_address_count_)) &
        (uint32_t)(0x0000ffff);
    submission_queue_entry->prp_entry1 = (DATA_MEMORY_REGION);  // dummy address
    submission_queue_entry->prp_entry2 =
        (DATA_MEMORY_REGION + 0x1000);  // dummy address
  }

  return submission_queue_entry;
}

void IOFlowBase::SubmitIORequest(HostIORequest* request) {
  if (NVME_SQ_FULL(nvme_queue_pair_) || available_command_ids_.size() == 0) {
    waiting_requests_.push_back(request);
  } else {
    if (nvme_software_request_queue_[*available_command_ids_.begin()] != NULL) {
      PRINT_ERROR(
          "Unexpteced situation in IO_Flow_Base! Overwriting an unhandled "
          "I/O request in the queue!")
    } else {
      request->io_queue_info_ = *available_command_ids_.begin();
      nvme_software_request_queue_[*available_command_ids_.begin()] = request;
      available_command_ids_.erase(available_command_ids_.begin());
      request_queue_in_memory_[nvme_queue_pair_.submission_queue_tail] =
          request;
      NVME_UPDATE_SQ_TAIL(nvme_queue_pair_);
    }
    request->enqueue_time_ = Simulator->Time();
    pcie_root_complex_->WriteToDevice(
        nvme_queue_pair_.submission_tail_register_address_on_device,
        nvme_queue_pair_
            .submission_queue_tail);  // Based on NVMe protocol definition,
                                      // the updated tail pointer should be
                                      // informed to the device
  }
}

void IOFlowBase::NVMeUpdateAndSubmitCompletionQueueTail() {
  nvme_queue_pair_.completion_queue_head++;
  if (nvme_queue_pair_.completion_queue_head ==
      nvme_queue_pair_.completion_queue_size) {
    nvme_queue_pair_.completion_queue_head = 0;
  }
  pcie_root_complex_->WriteToDevice(
      nvme_queue_pair_.completion_head_register_address_on_device,
      nvme_queue_pair_.completion_queue_head);
}

const NVMeQueuePair* IOFlowBase::get_nvme_queue_pair() {
  return &nvme_queue_pair_;
}

LogicalHostAddressType
IOFlowBase::get_start_logcical_sector_address_on_device() {
  return start_logical_sector_address_on_device_;
}

LogicalHostAddressType IOFlowBase::get_end_logical_sector_address_on_device() {
  return end_logical_sector_address_on_device_;
}

uint32_t IOFlowBase::get_generated_request_count() {
  return stat_generated_request_count_;
}

uint32_t IOFlowBase::get_serviced_request_count() {
  return stat_serviced_request_count_;
}

uint32_t IOFlowBase::get_device_response_time() {
  if (stat_serviced_request_count_ == 0) {
    return 0;
  }

  return (uint32_t)(stat_sum_device_response_time_ /
                    stat_serviced_request_count_ /
                    SIM_TIME_TO_MICROSECONDS_COEFF);
}

uint32_t IOFlowBase::get_min_device_response_time() {
  return (uint32_t)(stat_min_device_response_time_ /
                    SIM_TIME_TO_MICROSECONDS_COEFF);
}

uint32_t IOFlowBase::get_max_device_reponse_time() {
  return (uint32_t)(stat_max_device_response_time_ /
                    SIM_TIME_TO_MICROSECONDS_COEFF);
}

uint32_t IOFlowBase::get_end_to_end_request_delay() {
  if (stat_serviced_request_count_ == 0) {
    return 0;
  }

  return (uint32_t)(stat_sum_request_delay_ / stat_serviced_request_count_ /
                    SIM_TIME_TO_MICROSECONDS_COEFF);
}

uint32_t IOFlowBase::get_min_end_to_end_request_delay() {
  return (uint32_t)(stat_min_request_delay_ / SIM_TIME_TO_MICROSECONDS_COEFF);
}

uint32_t IOFlowBase::get_max_end_to_end_request_delay() {
  return (uint32_t)(stat_max_request_delay_ / SIM_TIME_TO_MICROSECONDS_COEFF);
}

uint32_t IOFlowBase::get_device_response_time_short_term() {
  if (stat_serviced_request_count_short_term_ == 0) {
    return 0;
  }

  return (uint32_t)(stat_sum_device_response_time_short_term_ /
                    stat_serviced_request_count_short_term_ /
                    SIM_TIME_TO_MICROSECONDS_COEFF);
}

uint32_t IOFlowBase::get_end_to_end_request_delay_short_term() {
  if (stat_serviced_request_count_ == 0) {
    return 0;
  }

  return (uint32_t)(stat_sum_request_delay_short_term_ /
                    stat_serviced_request_count_short_term_ /
                    SIM_TIME_TO_MICROSECONDS_COEFF);
}

}  // namespace host_components