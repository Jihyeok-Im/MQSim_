#include "PCIeSwitch.h"

namespace host_components {
PCIeSwitch::PCIeSwitch(PCIeLink* pcie_link,
                       ssd_components::HostInterface* host_interface)
    : pcie_link_(pcie_link), host_interface_(host_interface) {}

void PCIeSwitch::DeliverToDevice(PCIeMessage* message) {
  host_interface_->ConsumePCIeMessage(message);
}

void PCIeSwitch::SendToHost(PCIeMessage* message) {
  pcie_link_->Deliver(message);
}

void PCIeSwitch::AttachSSDDevice(
    ssd_components::HostInterface* host_interface) {
  this->host_interface_ = host_interface;
}

bool PCIeSwitch::IsSSDConnected() { return this->host_interface_ != NULL; }

}  // namespace host_components