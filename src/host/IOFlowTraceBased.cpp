#include "IOFlowTraceBased.h"

#include "../utils/DistributionTypes.h"
#include "../utils/StringTools.h"
#include "ASCIITraceDef.h"

namespace host_components {
IOFlowTraceBased::IOFlowTraceBased(
    const SimObjectID &name, uint16_t flow_id,
    LogicalHostAddressType start_logical_sector_address_on_device,
    LogicalHostAddressType end_logical_sector_address_on_device,
    uint16_t io_queue_id, uint16_t nvme_submission_queue_size,
    uint16_t nvme_completion_queue_size,
    IOFlowPriorityClass::Priority priority_class,
    double initial_occupancy_ratio, std::string trace_file_path,
    TraceTimeUnit time_unit, unsigned int total_replay_count,
    unsigned int percentage_to_be_simulated, PCIeRootComplex *pcie_root_complex,
    bool enabled_logging, SimTime logging_period, std::string logging_file_path)
    : IOFlowBase(name, flow_id, start_logical_sector_address_on_device,
                 end_logical_sector_address_on_device, io_queue_id,
                 nvme_submission_queue_size, nvme_completion_queue_size,
                 priority_class, 0, initial_occupancy_ratio, 0,
                 pcie_root_complex, enabled_logging, logging_period,
                 logging_file_path),
      trace_file_path_(trace_file_path),
      time_unit_(time_unit),
      total_replay_no_(total_replay_count),
      percentage_to_be_simulated_(percentage_to_be_simulated),
      total_requests_in_file_(0),
      time_offset_(0) {
  if (percentage_to_be_simulated_ > 100) {
    percentage_to_be_simulated_ = 100;
    PRINT_MESSAGE(
        "Bad value for percentage of trace file! It is set to 100 % ");
  }
}

IOFlowTraceBased::~IOFlowTraceBased() {}

HostIORequest *IOFlowTraceBased::GenerateNextRequest() {
  if (current_trace_line_.size() == 0 ||
      stat_generated_request_count_ >= total_requests_to_be_generated_) {
    return NULL;
  }

  HostIORequest *request = new HostIORequest;
  if (current_trace_line_[ASCII_TRACE_TYPE_COLUMN].compare(
          ASCII_TRACE_WRITE_CODE) == 0) {
    request->type_ = HostIORequestType::kWrite;
    stat_generated_write_request_count_++;
  } else {
    request->type_ = HostIORequestType::kRead;
    stat_generated_read_request_count_++;
  }

  char *pEnd;
  request->logical_block_address_count_ = std::strtoul(
      current_trace_line_[ASCII_TRACE_SIZE_COLUMN].c_str(), &pEnd, 0);

  request->start_logical_block_address_ = std::strtoull(
      current_trace_line_[ASCII_TRACE_ADDRESS_COLUMN].c_str(), &pEnd, 0);
  if (request->start_logical_block_address_ <=
      (end_logical_sector_address_on_device_ -
       start_logical_sector_address_on_device_)) {
    request->start_logical_block_address_ +=
        start_logical_sector_address_on_device_;
  } else {
    request->start_logical_block_address_ =
        start_logical_sector_address_on_device_ +
        request->start_logical_block_address_ %
            (end_logical_sector_address_on_device_ -
             start_logical_sector_address_on_device_);
  }

  request->arrival_time_ = time_offset_ + Simulator->Time();
  stat_generated_request_count_++;

  return request;
}

void IOFlowTraceBased::NVMeConsumeIORequest(CompletionQueueEntry *io_request) {
  IOFlowBase::NVMeConsumeIORequest(io_request);
  IOFlowBase::NVMeUpdateAndSubmitCompletionQueueTail();
}

void IOFlowTraceBased::StartSimulation() {
  IOFlowBase::StartSimulation();
  std::string trace_line;
  char *pEnd;

  trace_file_.open(trace_file_path_, std::ios::in);
  if (!trace_file_.is_open()) {
    PRINT_ERROR("Error while opening input trace file: " << trace_file_path_)
  }
  PRINT_MESSAGE("Investigating input trace file: " << trace_file_path_);

  SimTime last_request_arrival_time = 0;
  while (std::getline(trace_file_, trace_line)) {
    Utils::Helper_Functions::Remove_cr(trace_line);
    current_trace_line_.clear();
    Utils::Helper_Functions::Tokenize(trace_line, ASCII_TRACE_LINE_DELIMITER,
                                      current_trace_line_);
    if (current_trace_line_.size() != ASCII_TRACE_ITEMS_PER_LINE) {
      break;
    }
    total_requests_in_file_++;
    SimTime prev_time = last_request_arrival_time;
    last_request_arrival_time = std::strtoll(
        current_trace_line_[ASCII_TRACE_TIME_COLUMN].c_str(), &pEnd, 10);
    if (last_request_arrival_time < prev_time) {
      PRINT_ERROR("Unexpected request arrival time: "
                  << last_request_arrival_time
                  << "\nMQSim expects request arrival times to be "
                     "monotonically increasing in the input trace!")
    }
  }

  trace_file_.close();
  PRINT_MESSAGE("Trace file: " << trace_file_path_ << " seems healthy");

  if (total_replay_no_ == 1) {
    total_requests_to_be_generated_ =
        (int)(((double)percentage_to_be_simulated_ / 100) *
              total_requests_in_file_);
  } else {
    total_requests_to_be_generated_ =
        total_requests_in_file_ * total_replay_no_;
  }

  trace_file_.open(trace_file_path_);
  current_trace_line_.clear();
  std::getline(trace_file_, trace_line);
  Utils::Helper_Functions::Remove_cr(trace_line);
  Utils::Helper_Functions::Tokenize(trace_line, ASCII_TRACE_LINE_DELIMITER,
                                    current_trace_line_);
  Simulator->RegisterSimulatorEvent(
      std::strtoll(current_trace_line_[ASCII_TRACE_TIME_COLUMN].c_str(), &pEnd,
                   10),
      this);
}

void IOFlowTraceBased::ValidateSimulationConfig() {}

void IOFlowTraceBased::ExecuteSimulatorEvent(sim_engine::SimEvent *) {
  HostIORequest *request = GenerateNextRequest();
  if (request != NULL) {
    SubmitIORequest(request);
  }

  if (stat_generated_request_count_ < total_requests_to_be_generated_) {
    std::string trace_line;
    if (std::getline(trace_file_, trace_line)) {
      Utils::Helper_Functions::Remove_cr(trace_line);
      current_trace_line_.clear();
      Utils::Helper_Functions::Tokenize(trace_line, ASCII_TRACE_LINE_DELIMITER,
                                        current_trace_line_);
    } else {
      trace_file_.close();
      trace_file_.open(trace_file_path_);
      replay_counter_++;
      time_offset_ = Simulator->Time();
      std::getline(trace_file_, trace_line);
      Utils::Helper_Functions::Remove_cr(trace_line);
      current_trace_line_.clear();
      Utils::Helper_Functions::Tokenize(trace_line, ASCII_TRACE_LINE_DELIMITER,
                                        current_trace_line_);
      PRINT_MESSAGE("* Replay round " << replay_counter_ << "of "
                                      << total_replay_no_ << " started  for"
                                      << get_id())
    }
    char *pEnd;
    Simulator->RegisterSimulatorEvent(
        time_offset_ +
            std::strtoll(current_trace_line_[ASCII_TRACE_TIME_COLUMN].c_str(),
                         &pEnd, 10),
        this);
  }
}

// void IOFlowTraceBased::Get_statistics(
//     Utils::WorkloadStatistics &stats,
//     LPA_type (*Convert_host_logical_address_to_device_address)(LHA_type lha),
//     page_status_type (*Find_NVM_subunit_access_bitmap)(LHA_type lha)) {
//   stats.Type = Utils::Workload_Type::TRACE_BASED;
//   stats.Stream_id =
//       io_queue_id - 1;  // In MQSim, there is a simple relation between
//       stream
//                         // id and the io_queue_id of NVMe
//   stats.Min_LHA = start_lsa_on_device;
//   stats.Max_LHA = end_lsa_on_device;
//   for (int i = 0; i < MAX_ARRIVAL_TIME_HISTOGRAM + 1; i++) {
//     stats.Write_arrival_time.push_back(0);
//     stats.Read_arrival_time.push_back(0);
//   }
//   for (int i = 0; i < MAX_REQSIZE_HISTOGRAM_ITEMS + 1; i++) {
//     stats.Write_size_histogram.push_back(0);
//     stats.Read_size_histogram.push_back(0);
//   }
//   stats.Total_generated_requests = 0;
//   stats.Total_accessed_lbas = 0;

//   std::ifstream trace_file_temp;
//   trace_file_temp.open(trace_file_path, std::ios::in);
//   if (!trace_file_temp.is_open()) {
//     PRINT_ERROR("Error while opening the input trace file!")
//   }

//   std::string trace_line;
//   char *pEnd;
//   SimTimeType last_request_arrival_time = 0;
//   SimTimeType sum_inter_arrival = 0;
//   uint64_t sum_request_size = 0;
//   std::vector<std::string> line_splitted;
//   while (std::getline(trace_file_temp, trace_line)) {
//     Utils::Helper_Functions::Remove_cr(trace_line);
//     line_splitted.clear();
//     Utils::Helper_Functions::Tokenize(trace_line, ASCII_TRACE_LINE_DELIMITER,
//                                       line_splitted);
//     if (line_splitted.size() != ASCII_TRACE_ITEMS_PER_LINE) {
//       break;
//     }
//     SimTimeType prev_time = last_request_arrival_time;
//     last_request_arrival_time =
//         std::strtoull(line_splitted[ASCIITraceTimeColumn].c_str(), &pEnd,
//         10);
//     if (last_request_arrival_time < prev_time) {
//       PRINT_ERROR("Unexpected request arrival time: "
//                   << last_request_arrival_time
//                   << "\nMQSim expects request arrival times to be monotonic "
//                      "increasing in the input trace!")
//     }
//     SimTimeType diff =
//         (last_request_arrival_time - prev_time) /
//         1000;  // The arrival rate histogram is stored in the microsecond
//         unit
//     sum_inter_arrival += last_request_arrival_time - prev_time;

//     unsigned int LBA_count =
//         std::strtoul(line_splitted[ASCIITraceSizeColumn].c_str(), &pEnd, 0);
//     sum_request_size += LBA_count;
//     LogicalHostAddressType start_LBA =
//         std::strtoull(line_splitted[ASCIITraceAddressColumn].c_str(), &pEnd,
//         0);
//     if (start_LBA <= (end_lsa_on_device - start_lsa_on_device)) {
//       start_LBA += start_lsa_on_device;
//     } else {
//       start_LBA = start_lsa_on_device +
//                   start_LBA % (end_lsa_on_device - start_lsa_on_device);
//     }
//     LogicalHostAddressType end_LBA = start_LBA + LBA_count - 1;
//     if (end_LBA > end_lsa_on_device) {
//       end_LBA = start_lsa_on_device + (end_LBA - end_lsa_on_device) - 1;
//     }

//     // Address access pattern statistics
//     while (start_LBA <= end_LBA) {
//       LogicalHostAddressType device_address =
//           Convert_host_logical_address_to_device_address(start_LBA);
//       page_status_type access_status_bitmap =
//           Find_NVM_subunit_access_bitmap(start_LBA);
//       if
//       (line_splitted[ASCII_TRACE_TYPE_COLUMN].compare(ASCII_TRACE_WRITE_CODE)
//       ==
//           0) {
//         if (stats.Write_address_access_pattern.find(device_address) ==
//             stats.Write_address_access_pattern.end()) {
//           Utils::Address_Histogram_Unit hist;
//           hist.Access_count = 1;
//           hist.Accessed_sub_units = access_status_bitmap;
//           stats.Write_address_access_pattern[device_address] = hist;
//         } else {
//           stats.Write_address_access_pattern[device_address].Access_count =
//               stats.Write_address_access_pattern[device_address].Access_count
//               + 1;
//           stats.Write_address_access_pattern[device_address]
//               .Accessed_sub_units =
//               stats.Write_address_access_pattern[device_address]
//                   .Accessed_sub_units |
//               access_status_bitmap;
//         }

//         if (stats.Read_address_access_pattern.find(device_address) !=
//             stats.Read_address_access_pattern.end()) {
//           stats.Write_read_shared_addresses.insert(device_address);
//         }
//       } else {
//         if (stats.Read_address_access_pattern.find(device_address) ==
//             stats.Read_address_access_pattern.end()) {
//           Utils::Address_Histogram_Unit hist;
//           hist.Access_count = 1;
//           hist.Accessed_sub_units = access_status_bitmap;
//           stats.Read_address_access_pattern[device_address] = hist;
//         } else {
//           stats.Read_address_access_pattern[device_address].Access_count =
//               stats.Read_address_access_pattern[device_address].Access_count
//               + 1;
//           stats.Read_address_access_pattern[device_address].Accessed_sub_units
//           =
//               stats.Read_address_access_pattern[device_address]
//                   .Accessed_sub_units |
//               access_status_bitmap;
//         }

//         if (stats.Write_address_access_pattern.find(device_address) !=
//             stats.Write_address_access_pattern.end()) {
//           stats.Write_read_shared_addresses.insert(device_address);
//         }
//       }
//       stats.Total_accessed_lbas++;
//       start_LBA++;
//       if (start_LBA > end_lsa_on_device) {
//         start_LBA = start_lsa_on_device;
//       }
//     }

//     // Request size statistics
//     if
//     (line_splitted[ASCII_TRACE_TYPE_COLUMN].compare(ASCII_TRACE_WRITE_CODE)
//     == 0) {
//       if (diff < MAX_ARRIVAL_TIME_HISTOGRAM) {
//         stats.Write_arrival_time[diff]++;
//       } else {
//         stats.Write_arrival_time[MAX_ARRIVAL_TIME_HISTOGRAM]++;
//       }

//       if (LBA_count < MAX_REQSIZE_HISTOGRAM_ITEMS) {
//         stats.Write_size_histogram[LBA_count]++;
//       } else {
//         stats.Write_size_histogram[MAX_REQSIZE_HISTOGRAM_ITEMS]++;
//       }
//     } else {
//       if (diff < MAX_ARRIVAL_TIME_HISTOGRAM) {
//         stats.Read_arrival_time[diff]++;
//       } else {
//         stats.Read_arrival_time[MAX_ARRIVAL_TIME_HISTOGRAM]++;
//       }

//       if (LBA_count < MAX_REQSIZE_HISTOGRAM_ITEMS) {
//         stats.Read_size_histogram[LBA_count]++;
//       } else {
//         stats.Read_size_histogram[(unsigned
//         int)MAX_REQSIZE_HISTOGRAM_ITEMS]++;
//       }
//     }
//     stats.Total_generated_requests++;
//   }
//   trace_file_temp.close();
//   stats.Average_request_size_sector =
//       (unsigned int)(sum_request_size / stats.Total_generated_requests);
//   stats.Average_inter_arrival_time_nano_sec =
//       sum_inter_arrival / stats.Total_generated_requests;

//   stats.Initial_occupancy_ratio = initial_occupancy_ratio_;
//   stats.Replay_no = total_replay_no_;
// }
}  // namespace host_components