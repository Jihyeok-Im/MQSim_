#ifndef USER_REQUEST_H_
#define USER_REQUEST_H_

#include <list>
#include <string>

#include "../sim/SimDef.h"
#include "FlashTransaction.h"
#include "HostInterfaceDef.h"
#include "SSDDef.h"

namespace ssd_components {
enum class UserRequestType { kRead, kWrite };
class FlashTransaction;
class UserRequest {
 public:
  UserRequest();

  IOFlowPriorityClass::Priority priority_class_;
  IORequestIDType id_;
  LogicalHostAddressType start_logical_block_address_;

  SimTime stat_initiation_time_;
  SimTime stat_response_time_;
  std::list<FlashTransaction*> transaction_list_;
  unsigned int sectors_serviced_from_cache_;

  unsigned int size_in_byte_;
  unsigned int size_in_sector_;
  UserRequestType request_type_;
  StreamID stream_id_;
  bool is_ignored_;
  void* io_command_info_;  // used to store host io command info
  void* data_;

 private:
  static unsigned int lastID;
};
}  // namespace ssd_components

#endif