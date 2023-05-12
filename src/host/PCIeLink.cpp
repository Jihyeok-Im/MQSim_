#include "PCIeLink.h"

#include "../sim/Engine.h"
#include "../sim/SimDef.h"
#include "PCIeMessage.h"

namespace host_components {
PCIeLink::PCIeLink(const SimObjectID& id, PCIeRootComplex* pcie_root_complex,
                   PCIeSwitch* pcie_switch, double lane_bandwidth_gbps,
                   int lane_count, int tlp_header_size,
                   int tlp_max_payload_size, int dllp_overhead, int ph_overhead)
    : SimObject(id),
      pcie_root_complex_(pcie_root_complex),
      pcie_switch_(pcie_switch),
      lane_bandwidth_gbps_(lane_bandwidth_gbps),
      lane_count_(lane_count),
      tlp_header_size_(tlp_header_size),
      tlp_max_payload_size_(tlp_max_payload_size),
      dllp_overhead_(dllp_overhead),
      ph_overhead_(ph_overhead) {
  packet_overhead_ = ph_overhead_ + dllp_overhead_ + tlp_header_size_;
}

void PCIeLink::set_pcie_root_complex(PCIeRootComplex* pcie_root_complex) {
  this->pcie_root_complex_ = pcie_root_complex;
}

void PCIeLink::set_pcie_switch(PCIeSwitch* pcie_switch) {
  this->pcie_switch_ = pcie_switch;
}

void PCIeLink::Deliver(PCIeMessage* message) {
  switch (message->get_destination()) {
    case PCIeDestinationType::kHost:
      message_buffer_toward_root_complex_.push(message);
      if (message_buffer_toward_root_complex_.size() > 1) {
        return;  // active transfers exist
      }
      Simulator->RegisterSimulatorEvent(
          Simulator->Time() + EstimateTransferTime(message), this,
          (void*)(intptr_t)PCIeDestinationType::kDevice,
          static_cast<int>(PCIeLinkEventType::kDeliver));
      break;
    case PCIeDestinationType::kDevice:
      message_buffer_toward_ssd_device_.push(message);
      if (message_buffer_toward_ssd_device_.size() > 1) {
        return;
      }
      Simulator->RegisterSimulatorEvent(
          Simulator->Time() + EstimateTransferTime(message), this,
          (void*)(intptr_t)PCIeDestinationType::kDevice,
          static_cast<int>(PCIeLinkEventType::kDeliver));
      break;
    default:
      break;
  }
}

void PCIeLink::StartSimulation() {}

void PCIeLink::ValidateSimulationConfig() {}

void PCIeLink::ExecuteSimulatorEvent(sim_engine::SimEvent* event) {
  PCIeMessage* message = NULL;
  PCIeDestinationType desination =
      (PCIeDestinationType)(intptr_t)event->get_parameters();
  switch (desination) {
    case PCIeDestinationType::kHost:
      message = message_buffer_toward_root_complex_.front();
      message_buffer_toward_root_complex_.pop();
      pcie_root_complex_->ConsumePCIeMessage(message);
      if (message_buffer_toward_root_complex_.size() > 0) {
        Simulator->RegisterSimulatorEvent(
            Simulator->Time() +
                EstimateTransferTime(
                    message_buffer_toward_root_complex_.front()),
            this, (void*)(intptr_t)PCIeDestinationType::kHost,
            static_cast<int>(PCIeLinkEventType::kDeliver));
      }
      break;
    case PCIeDestinationType::kDevice:
      message = message_buffer_toward_ssd_device_.front();
      message_buffer_toward_ssd_device_.pop();
      pcie_switch_->DeliverToDevice(message);
      if (message_buffer_toward_ssd_device_.size() > 0) {
        Simulator->RegisterSimulatorEvent(
            Simulator->Time() +
                EstimateTransferTime(message_buffer_toward_ssd_device_.front()),
            this, (void*)(intptr_t)PCIeDestinationType::kDevice,
            static_cast<int>(PCIeLinkEventType::kDeliver));
      }
      break;
  }
}

}  // namespace host_components