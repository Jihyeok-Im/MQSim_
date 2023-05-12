#include "PCIeRootComplex.h"

namespace host_components {
PCIeRootComplex::PCIeRootComplex(
    PCIeLink* pcie_link, std::vector<host_components::IOFlowBase*>* io_flows)
    : pcie_link_(pcie_link), io_flows_(io_flows) {}

void PCIeRootComplex::WriteToMemory(const uint64_t address,
                                    const void* payload) {
  if (address >= DATA_MEMORY_REGION) {
    // nothing to do
  } else {
    unsigned int flow_id =
        QUEUE_ID_TO_FLOW_ID(((CompletionQueueEntry*)payload)->sq_identifier);
    ((*io_flows_)[flow_id])
        ->NVMeConsumeIORequest((CompletionQueueEntry*)payload);
  }
}

void PCIeRootComplex::WriteToDevice(uint64_t address, uint16_t write_value) {
  PCIeMessage* pcie_message = new host_components::PCIeMessage;
  pcie_message->set_message_type(PCIeMessageType::kWriteRequest);
  pcie_message->set_destination(host_components::PCIeDestinationType::kDevice);
  pcie_message->set_address(address);
  pcie_message->set_payload((void*)(intptr_t)write_value);
  pcie_message->set_payload_size(sizeof(write_value));
  pcie_link_->Deliver(pcie_message);
}

void PCIeRootComplex::ReadFromMemory(const uint64_t address,
                                     const unsigned int read_size) {
  PCIeMessage* pcie_message = new host_components::PCIeMessage;
  pcie_message->set_message_type(PCIeMessageType::kReadComplete);
  pcie_message->set_destination(host_components::PCIeDestinationType::kDevice);
  pcie_message->set_address(address);

  if (address >= DATA_MEMORY_REGION) {
    pcie_message->set_payload(NULL);  // dummy for stand alone mode
    pcie_message->set_payload_size(read_size);
  } else {
    uint16_t flow_id =
        QUEUE_ID_TO_FLOW_ID(uint16_t(address >> NVME_COMP_Q_MEMORY_REGION));
    pcie_message->set_payload(
        (*io_flows_)[flow_id]->NVMeReadSubmissionQueueEntry(address));
    pcie_message->set_payload_size(sizeof(SubmissionQueueEntry));
  }
  pcie_link_->Deliver(pcie_message);
}

void PCIeRootComplex::SetIOFlows(
    std::vector<host_components::IOFlowBase*>* io_flows) {
  this->io_flows_ = io_flows;
}

}  // namespace host_components