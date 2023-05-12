#ifndef FLASH_TRANSACTION_H_
#define FLASH_TRANSACTION_H_

#include <cstdint>
#include <list>
#include <string>

#include "../nand_flash_memory/NANDFlashDef.h"
#include "../nand_flash_memory/Package.h"
#include "../nand_flash_memory/PhysicalPageAddress.h"
#include "../sim/Engine.h"
#include "../sim/SimDef.h"
#include "../sim/SimEvent.h"
#include "UserRequest.h"

namespace ssd_components {
class UserRequest;

enum class TransactionType { kRead, kWrite, kErase, kUnknown };
enum class TransactionSourceType { kUserIO, kCache, kGCWL, kMapping };

class FlashTransaction {
 public:
  FlashTransaction(StreamID stream_id, TransactionSourceType source,
                   TransactionType type, UserRequest* user_request,
                   IOFlowPriorityClass::Priority priority_class,
                   unsigned int data_size_in_byte, LPA logical_page_address,
                   PPA physical_page_address);

  FlashTransaction(StreamID stream_id, TransactionSourceType source,
                   TransactionType type, UserRequest* user_request,
                   IOFlowPriorityClass::Priority priority_class,
                   unsigned int data_size_in_byte, LPA logical_page_address,
                   PPA physical_page_address,
                   const nand_flash_memory::PhysicalPageAddress& address);

 protected:
  StreamID stream_id_;
  TransactionSourceType source_;
  TransactionType type_;
  UserRequest* user_request_;
  IOFlowPriorityClass::Priority priority_class_;
  SimTime issue_time_;
  SimTime stat_execution_time_, stat_transfer_time_;

  nand_flash_memory::PhysicalPageAddress address_;
  unsigned int data_and_metadata_size_in_byte_;
  LPA logical_page_address_;
  PPA physical_page_address_;

  bool physical_address_determined_;
};
}  // namespace ssd_components

#endif