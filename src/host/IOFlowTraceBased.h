#ifndef IO_FLOW_TRACE_BASED_H_
#define IO_FLOW_TRACE_BASED_H_

#include <fstream>
#include <iostream>
#include <string>

#include "ASCIITraceDef.h"
#include "IOFlowBase.h"

namespace host_components {
class IOFlowTraceBased : public IOFlowBase {
 public:
  IOFlowTraceBased(const SimObjectID &name, uint16_t flow_id,
                   LogicalHostAddressType start_lsa_on_device,
                   LogicalHostAddressType end_lsa_on_device,
                   uint16_t io_queue_id, uint16_t nvme_submission_queue_size,
                   uint16_t nvme_completion_queue_size,
                   IOFlowPriorityClass::Priority priority_class,
                   double initial_occupancy_ratio, std::string trace_file_path,
                   TraceTimeUnit time_unit, unsigned int total_replay_count,
                   unsigned int percentage_to_be_simulated,
                   PCIeRootComplex *pcie_root_complex, bool enabled_logging,
                   SimTime logging_period, std::string logging_file_path);
  ~IOFlowTraceBased();
  HostIORequest *GenerateNextRequest();
  void NVMeConsumeIORequest(CompletionQueueEntry *);
  void StartSimulation();
  void ValidateSimulationConfig();
  void ExecuteSimulatorEvent(sim_engine::SimEvent *);
  void Get_statistics();

 private:
  TraceTimeUnit time_unit_;
  unsigned int percentage_to_be_simulated_;
  std::string trace_file_path_;
  std::ifstream trace_file_;
  unsigned int total_replay_no_, replay_counter_;
  unsigned int total_requests_in_file_;
  std::vector<std::string> current_trace_line_;
  SimTime time_offset_;
};
}  // namespace host_components

#endif  // !IO_FLOW_TRACE_BASED_H
