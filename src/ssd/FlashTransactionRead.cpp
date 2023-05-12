#include "FlashTransactionRead.h"

#include "../nand_flash_memory/NANDFlashDef.h"

namespace ssd_components {
FlashTransactionRead::FlashTransactionRead(
    TransactionSourceType source, StreamID stream_id,
    unsigned int data_size_in_byte, LPA logical_page_address,
    PPA physical_page_address,
    ssd_components::UserRequest* related_user_request, MemoryContent content,
    FlashTransactionWrite* related_write, PageStatus read_sectors_bitmap,
    DataTimeStamp data_time_stamp)
    : FlashTransaction(stream_id, source, TransactionType::kRead,
                       related_user_request, IOFlowPriorityClass::kUndefined,
                       data_size_in_byte, logical_page_address,
                       physical_page_address),
      content_(content),
      related_write_(related_write),
      read_sectors_bitmap_(read_sectors_bitmap),
      data_time_stamp_(data_time_stamp) {}

FlashTransactionRead::FlashTransactionRead(
    TransactionSourceType source, StreamID stream_id,
    unsigned int data_size_in_byte, LPA logical_page_address,
    PPA physical_page_address,
    const nand_flash_memory::PhysicalPageAddress& address,
    ssd_components::UserRequest* related_user_request, MemoryContent content,
    FlashTransactionWrite* related_write, PageStatus read_sectors_bitmap,
    DataTimeStamp data_time_stamp)
    : FlashTransaction(stream_id, source, TransactionType::kRead,
                       related_user_request, IOFlowPriorityClass::kUndefined,
                       data_size_in_byte, logical_page_address,
                       physical_page_address, address),
      content_(content),
      related_write_(related_write),
      read_sectors_bitmap_(read_sectors_bitmap),
      data_time_stamp_(data_time_stamp) {}

FlashTransactionRead::FlashTransactionRead(
    TransactionSourceType source, StreamID stream_id,
    unsigned int data_size_in_byte, LPA logical_page_address,
    PPA physical_page_address,
    ssd_components::UserRequest* related_user_request,
    IOFlowPriorityClass::Priority priority_class, MemoryContent content,
    PageStatus read_sectors_bitmap, DataTimeStamp data_time_stamp)
    : FlashTransaction(stream_id, source, TransactionType::kRead,
                       related_user_request, IOFlowPriorityClass::kUndefined,
                       data_size_in_byte, logical_page_address,
                       physical_page_address),
      content_(content),
      related_write_(NULL),
      read_sectors_bitmap_(read_sectors_bitmap),
      data_time_stamp_(data_time_stamp) {}

FlashTransactionRead::FlashTransactionRead(
    TransactionSourceType source, StreamID stream_id,
    unsigned int data_size_in_byte, LPA logical_page_address,
    PPA physical_page_address,
    ssd_components::UserRequest* related_user_request, MemoryContent content,
    PageStatus read_sectors_bitmap, DataTimeStamp data_time_stamp)
    : FlashTransaction(stream_id, source, TransactionType::kRead,
                       related_user_request, IOFlowPriorityClass::kUndefined,
                       data_size_in_byte, logical_page_address,
                       physical_page_address),
      content_(content),
      related_write_(NULL),
      read_sectors_bitmap_(read_sectors_bitmap),
      data_time_stamp_(data_time_stamp) {}
}  // namespace ssd_components