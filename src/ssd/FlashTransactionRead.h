#ifndef FLASH_TRANSACTION_READ_H_
#define FLASH_TRANSACTION_READ_H_

#include "../nand_flash_memory/NANDFlashDef.h"
#include "FlashTransaction.h"

namespace ssd_components {
class FlashTransactionWrite;
class FlashTransaction;
class FlashTransactionRead : public FlashTransaction {
 public:
  FlashTransactionRead(TransactionSourceType source, StreamID streamd_id,
                       unsigned int data_size_in_btye, LPA logical_page_address,
                       PPA physical_page_address,
                       ssd_components::UserRequest* related_user_request,
                       MemoryContent content,
                       FlashTransactionWrite* related_write,
                       PageStatus read_sectors_bitmap,
                       DataTimeStamp data_time_stamp);
  FlashTransactionRead(TransactionSourceType source, StreamID stream_id,
                       unsigned int data_size_in_byte, LPA logical_page_address,
                       PPA physical_page_address,
                       const nand_flash_memory::PhysicalPageAddress& address,
                       ssd_components::UserRequest* related_user_request,
                       MemoryContent content,
                       FlashTransactionWrite* related_write,
                       PageStatus read_sectors_bitmap,
                       DataTimeStamp data_time_stamp);
  FlashTransactionRead(TransactionSourceType source, StreamID stream_id,
                       unsigned int data_size_in_byte, LPA logical_page_address,
                       PPA physical_page_address,
                       ssd_components::UserRequest* related_user_request,
                       IOFlowPriorityClass::Priority priority_class,
                       MemoryContent content, PageStatus read_sectors_bitmap,
                       DataTimeStamp data_time_stamp);
  FlashTransactionRead(TransactionSourceType source, StreamID stream_id,
                       unsigned int data_size_in_byte, LPA logical_page_address,
                       PPA physical_page_address,
                       ssd_components::UserRequest* related_user_request,
                       MemoryContent content, PageStatus read_sectors_bitmap,
                       DataTimeStamp data_time_stamp);
  MemoryContent content_;
  FlashTransactionWrite* related_write_;
  PageStatus read_sectors_bitmap_;
  DataTimeStamp data_time_stamp_;
};
}  // namespace ssd_components

#endif