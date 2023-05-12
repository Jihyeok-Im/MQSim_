#ifndef HOST_IO_REQUEST_H_
#define HOST_IO_REQUEST_H_

#include "../ssd/SSDDef.h"

namespace host_components {
enum class HostIORequestType { kRead, kWrite };
class HostIORequest {
 public:
  SimTime arrival_time_;  // The time that the request has been generated
  SimTime
      enqueue_time_;  // The time that the request enqueued into the I/O queue
  LogicalHostAddressType start_logical_block_address_;
  unsigned int logical_block_address_count_;
  HostIORequestType type_;
  uint16_t io_queue_info_;
};
}  // namespace host_components

#endif  // !HOST_IO_REQUEST_H
