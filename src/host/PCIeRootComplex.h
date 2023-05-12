#ifndef PCIE_ROOT_COMPLEX_H_
#define PCIE_ROOT_COMPLEX_H_

#include "../ssd/HostInterfaceDef.h"
#include "HostDef.h"
#include "HostIORequest.h"
#include "IOFlowBase.h"
#include "PCIeLink.h"
#include "PCIeMessage.h"

namespace host_components {
class PCIeLink;
class IOFlowBase;
class PCIeRootComplex {
 public:
  PCIeRootComplex(PCIeLink* pcie_link,
                  std::vector<host_components::IOFlowBase*>* io_flows);

  void ConsumePCIeMessage(PCIeMessage* message) {
    switch (message->get_message_type()) {
      case PCIeMessageType::kReadRequest:
        ReadFromMemory(message->get_address(),
                       (unsigned int)(intptr_t)message->get_payload());
        break;
      case PCIeMessageType::kWriteRequest:
        WriteToMemory(message->get_address(), message->get_payload());
        break;
      default:
        break;
    }
    delete message;
  }

  void WriteToDevice(uint64_t address, uint16_t write_value);
  void SetIOFlows(std::vector<host_components::IOFlowBase*>* io_flows);

 private:
  PCIeLink* pcie_link_;
  std::vector<host_components::IOFlowBase*>* io_flows_;

  void WriteToMemory(const uint64_t address, const void* payload);
  void ReadFromMemory(const uint64_t address, const unsigned int size);
};
}  // namespace host_components

#endif