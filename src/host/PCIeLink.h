#ifndef PCIE_LINK_H_
#define PCIE_LINK_H_

#include "../sim/SimDef.h"
#include "../sim/SimEvent.h"
#include "../sim/SimObject.h"
#include "PCIeMessage.h"
#include "PCIeRootComplex.h"
#include "PCIeSwitch.h"
#include "queue"

namespace host_components {
class PCIeSwitch;
class PCIeRootComplex;
enum class PCIeLinkEventType { kDeliver };

class PCIeLink : public sim_engine::SimObject {
 public:
  PCIeLink(const SimObjectID& id, PCIeRootComplex* pcie_root_complex,
           PCIeSwitch* pcie_switch, double lane_bandwidth_gbps = 1,
           int lane_count = 4, int tlp_header_size = 20,
           int tlp_max_payload_size = 128, int dllp_overhead = 6,
           int ph_overhead = 2);
  // tlp : Transaction layer packet
  // dllp : Data link layer packet
  // ph : physical layer

  void Deliver(PCIeMessage*);
  void StartSimulation();
  void ValidateSimulationConfig();
  void ExecuteSimulatorEvent(sim_engine::SimEvent*);
  void set_pcie_root_complex(PCIeRootComplex*);
  void set_pcie_switch(PCIeSwitch*);

 private:
  PCIeRootComplex* pcie_root_complex_;
  PCIeSwitch* pcie_switch_;
  double lane_bandwidth_gbps_;  // GB/s
  int lane_count_;
  int tlp_header_size_, tlp_max_payload_size_;
  int dllp_overhead_, ph_overhead_;
  int packet_overhead_;
  std::queue<PCIeMessage*> message_buffer_toward_ssd_device_;
  std::queue<PCIeMessage*> message_buffer_toward_root_complex_;

  SimTime EstimateTransferTime(PCIeMessage* message) {
    switch (message->get_message_type()) {
      case PCIeMessageType::kReadComplete:
      case PCIeMessageType::kWriteRequest: {
        int total_transferred_bytes =
            (message->get_payload_size() / tlp_max_payload_size_) *
                (tlp_max_payload_size_ + packet_overhead_) +
            (message->get_payload_size() % tlp_max_payload_size_ == 0
                 ? 0
                 : message->get_payload_size() % tlp_max_payload_size_ +
                       packet_overhead_);
        return (SimTime)(((double)((total_transferred_bytes / lane_count_) +
                                   (total_transferred_bytes % lane_count_ == 0
                                        ? 0
                                        : 1))) /
                         lane_bandwidth_gbps_);
      }
      case PCIeMessageType::kReadRequest:
        return (SimTime)((((packet_overhead_ + 4) / lane_count_) +
                          ((packet_overhead_ + 4) % lane_count_ == 0 ? 0 : 1)) /
                         lane_bandwidth_gbps_);
    }
    return 0;
  }
};

}  // namespace host_components

#endif