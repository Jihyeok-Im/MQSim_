#ifndef PCIE_SWITCH_H_
#define PCIE_SWITCH_H_

#include "../ssd/HostInterface.h"
#include "PCIeLink.h"
#include "PCIeMessage.h"

namespace ssd_components {
class HostInterface;
}

namespace host_components {
class PCIeLink;
class PCIeSwitch {
 public:
  PCIeSwitch(PCIeLink* pcie_link,
             ssd_components::HostInterface*
                 host_interface);  //, SSD_Components::Host_Interface_Base*
                                   // host_interface
  void DeliverToDevice(PCIeMessage*);
  void SendToHost(PCIeMessage*);
  void AttachSSDDevice(ssd_components::HostInterface* host_interface);
  bool IsSSDConnected();

 private:
  PCIeLink* pcie_link_;
  ssd_components::HostInterface* host_interface_;
};
}  // namespace host_components
#endif  //! PCIE_SWITCH_H
