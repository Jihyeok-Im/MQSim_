#ifndef FLASH_TRANSACTION_WRITE_H_
#define FLASH_TRANSACTION_WRITE_H_

#include "../nand_flash_memory/NANDFlashDef.h"
#include "FlashTransaction.h"
#include "FlashTransactionErase.h"
#include "FlashTransactionRead.h"

namespace ssd_components {
class FlashTransactionErase;
enum class WriteExecutionModeType { kSimple, kCopyback };

class FlashTransactionWrite : public FlashTransaction {
 public:
  FlashTransactionWrite(TransactionSourceType source, StreamID streamd_id,
                        unsigned int data_size_in_byte,
                        LPA logical_page_address, PPA physical_page_address,
                        ssd_components::UserRequest* related_user_request,
                        MemoryContent content,
                        FlashTransactionRead* related_read,
                        PageStatus write_sectors_bitmap,
                        DataTimeStamp data_timestamp);

  FlashTransactionWrite(TransactionSourceType source, StreamID streamd_id,
                        unsigned int data_size_in_byte,
                        LPA logical_page_address, PPA physical_page_address,
                        const nand_flash_memory::PhysicalPageAddress& address,
                        ssd_components::UserRequest* related_user_request,
                        MemoryContent content,
                        FlashTransactionRead* related_read,
                        PageStatus write_sectors_bitmap,
                        DataTimeStamp data_timestamp);

  FlashTransactionWrite(TransactionSourceType source, StreamID streamd_id,
                        unsigned int data_size_in_byte,
                        LPA logical_page_address,
                        ssd_components::UserRequest* related_user_request,
                        IOFlowPriorityClass::Priority priority_class,
                        MemoryContent content, PageStatus write_sectors_bitmap,
                        DataTimeStamp data_timestamp);

  FlashTransactionWrite(TransactionSourceType source, StreamID streamd_id,
                        unsigned int data_size_in_byte,
                        LPA logical_page_address,
                        ssd_components::UserRequest* related_user_request,
                        MemoryContent content, PageStatus write_sectors_bitmap,
                        DataTimeStamp data_timestamp);
  MemoryContent content_;
  FlashTransactionRead* related_read_;
  FlashTransactionErase* related_erase_;
  PageStatus write_sectors_bitmap_;
  DataTimeStamp data_time_stamp_;
  WriteExecutionModeType execution_mode_;
};
}  // namespace ssd_components

#endif