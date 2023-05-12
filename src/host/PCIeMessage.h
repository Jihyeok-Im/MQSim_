#ifndef PCIE_MESSAGE_H_
#define PCIE_MESSAGE_H_

#include <cstdint>

namespace host_components {
enum class PCIeDestinationType { kHost, kDevice };
enum class PCIeMessageType { kReadRequest, kWriteRequest, kReadComplete };

class PCIeMessage {
 public:
  PCIeDestinationType get_destination() { return destination_; }
  PCIeMessageType get_message_type() { return message_type_; }
  void* get_payload() { return payload_; }
  unsigned int get_payload_size() { return payload_size_; }
  uint64_t get_address() { return address_; }

  void set_destination(PCIeDestinationType destination) {
    this->destination_ = destination;
  }
  void set_message_type(PCIeMessageType message_type) {
    this->message_type_ = message_type;
  }
  void set_payload(void* payload) { this->payload_ = payload; }
  void set_payload_size(unsigned int payload_size) {
    this->payload_size_ = payload_size;
  }
  void set_address(uint64_t address) { this->address_ = address; }

 private:
  PCIeDestinationType destination_;
  PCIeMessageType message_type_;
  void* payload_;
  unsigned int payload_size_;
  uint64_t address_;
};
}  // namespace host_components
#endif