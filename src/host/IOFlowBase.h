#ifndef IO_FLOW_BASE_H_
#define IO_FLOW_BASE_H_

#include <iostream>
#include <list>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "../sim/SimDef.h"
#include "../sim/SimObject.h"
#include "../sim/SimReporter.h"
#include "../ssd/HostInterfaceDef.h"
#include "../ssd/SSDDef.h"
// #include "../utils/Workload_Statistics.h"
#include "HostIORequest.h"
#include "PCIeRootComplex.h"

namespace host_components {
struct NVMeQueuePair {
  uint16_t submission_queue_head;
  uint16_t submission_queue_tail;
  uint16_t submission_queue_size;
  uint64_t submission_tail_register_address_on_device;
  uint64_t submission_queue_memory_base_address;
  uint16_t completion_queue_head;
  uint16_t completion_queue_tail;
  uint16_t completion_queue_size;
  uint64_t completion_head_register_address_on_device;
  uint64_t completion_queue_memory_base_address;
};

#define NVME_SQ_FULL(Q)                                         \
  (Q.submission_queue_tail < Q.submission_queue_size - 1        \
       ? Q.submission_queue_tail + 1 == Q.submission_queue_head \
       : Q.submission_queue_head == 0)
#define NVME_UPDATE_SQ_TAIL(Q)                            \
  Q.submission_queue_tail++;                              \
  if (Q.submission_queue_tail == Q.submission_queue_size) \
    nvme_queue_pair_.submission_queue_tail = 0;

class PCIeRootComplex;

class IOFlowBase : public sim_engine::SimObject,
                   public sim_engine::SimReporter {
 public:
  IOFlowBase(const SimObjectID& id, uint16_t flow_id,
             LogicalHostAddressType start_logical_sector_address_on_device,
             LogicalHostAddressType end_logical_sector_address_on_device,
             uint16_t io_queue_id, uint16_t nvme_submission_queue_size,
             uint16_t nvme_completion_queue_size,
             IOFlowPriorityClass::Priority priority_class, SimTime stop_time,
             double initial_occupancy_ratio,
             unsigned int total_requests_to_be_generated,
             PCIeRootComplex* pcie_root_complex, bool enabled_logging,
             SimTime logging_peirod, std::string loggin_file_path);
  ~IOFlowBase();
  void StartSimulation();
  virtual HostIORequest* GenerateNextRequest() = 0;
  virtual void NVMeConsumeIORequest(CompletionQueueEntry*);
  SubmissionQueueEntry* NVMeReadSubmissionQueueEntry(uint64_t address);
  IOFlowPriorityClass::Priority get_priority_class() { return priority_class_; }
  const NVMeQueuePair* get_nvme_queue_pair();
  LogicalHostAddressType get_start_logcical_sector_address_on_device();
  LogicalHostAddressType get_end_logical_sector_address_on_device();
  uint32_t get_generated_request_count();
  uint32_t get_serviced_request_count();
  uint32_t get_device_response_time();
  uint32_t get_min_device_response_time();
  uint32_t get_max_device_reponse_time();
  uint32_t get_end_to_end_request_delay();
  uint32_t get_min_end_to_end_request_delay();
  uint32_t get_max_end_to_end_request_delay();
  void ReportResultsInXML(std::string name_prefix);
  virtual void get_statistics() = 0;

 protected:
  uint16_t flow_id_;
  double initial_occupancy_ratio_;
  SimTime stop_time_;
  unsigned int total_requests_to_be_generated_;
  PCIeRootComplex* pcie_root_complex_;
  LogicalHostAddressType start_logical_sector_address_on_device_,
      end_logical_sector_address_on_device_;
  IOFlowPriorityClass::Priority priority_class_;
  NVMeQueuePair nvme_queue_pair_;
  uint16_t io_queue_id_;
  uint16_t nvme_submission_queue_size_;
  uint16_t nvme_completion_queue_size_;
  std::set<uint16_t> available_command_ids_;
  std::vector<HostIORequest*> request_queue_in_memory_;
  std::list<HostIORequest*> waiting_requests_;
  std::unordered_map<SimTime, HostIORequest*> nvme_software_request_queue_;

  void SubmitIORequest(HostIORequest*);
  void NVMeUpdateAndSubmitCompletionQueueTail();

  // Variables used to collect statistics
  unsigned int stat_generated_request_count_,
      stat_generated_read_request_count_, stat_generated_write_request_count_;
  unsigned int stat_ignored_request_count_;
  unsigned int stat_serviced_request_count_, stat_serviced_read_request_count_,
      stat_serviced_write_request_count_;
  SimTime stat_sum_device_response_time_, stat_sum_device_response_time_read_,
      stat_sum_device_response_time_write_;
  SimTime stat_min_device_response_time_, stat_min_device_response_time_read_,
      stat_min_device_response_time_write_;
  SimTime stat_max_device_response_time_, stat_max_device_response_time_read_,
      stat_max_device_response_time_write_;
  SimTime stat_sum_request_delay_, stat_sum_request_delay_read_,
      stat_sum_request_delay_write_;
  SimTime stat_min_request_delay_, stat_min_request_delay_read_,
      stat_min_request_delay_write_;
  SimTime stat_max_request_delay_, stat_max_request_delay_read_,
      stat_max_request_delay_write_;
  SimTime stat_transferred_bytes_total_, stat_transferred_bytes_read_,
      stat_transferred_bytes_write_;
  int progress_;
  int next_progress_step_ = 0;

  // Variables used to log response time changes
  bool enabled_logging_;
  SimTime logging_period_;
  SimTime next_logging_milestone_;
  std::string logging_file_path_;
  std::ofstream log_file_;
  SimTime stat_sum_device_response_time_short_term_,
      stat_sum_request_delay_short_term_;
  unsigned int stat_serviced_request_count_short_term_;

  uint32_t get_device_response_time_short_term();      // in microseconds
  uint32_t get_end_to_end_request_delay_short_term();  // in microseconds
};
}  // namespace host_components

#endif