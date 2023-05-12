#include "FlashTransactionWrite.h"

namespace ssd_components {
FlashTransactionWrite::FlashTransactionWrite(
    TransactionSourceType source, StreamID stream_id,
    unsigned int data_size_in_byte, LPA logical_page_address,
    PPA physical_page_address,
    const nand_flash_memory::PhysicalPageAddress& address,
    ssd_components::UserRequest* related_user_request, MemoryContent content,
    FlashTransactionRead* related_read, PageStatus write_sectors_bitmap,
    DataTimeStamp data_time_stamp)
    : FlashTransaction(stream_id, source, TransactionType::kWrite,
                       related_user_request, IOFlowPriorityClass::kUndefined,
                       data_size_in_byte, logical_page_address,
                       physical_page_address, address),
      content_(content),
      related_read_(related_read),
      write_sectors_bitmap_(write_sectors_bitmap),
      data_time_stamp_(data_time_stamp),
      execution_mode_(WriteExecutionModeType::kSimple) {}

FlashTransactionWrite::FlashTransactionWrite(
    TransactionSourceType source, StreamID stream_id,
    unsigned int data_size_in_byte, LPA logical_page_address,
    PPA physical_page_address,
    ssd_components::UserRequest* related_user_request, MemoryContent content,
    FlashTransactionRead* related_read, PageStatus write_sectors_bitmap,
    DataTimeStamp data_time_stamp)
    : FlashTransaction(stream_id, source, TransactionType::kWrite,
                       related_user_request, IOFlowPriorityClass::kUndefined,
                       data_size_in_byte, logical_page_address,
                       physical_page_address),
      content_(content),
      related_read_(related_read),
      write_sectors_bitmap_(write_sectors_bitmap),
      data_time_stamp_(data_time_stamp),
      execution_mode_(WriteExecutionModeType::kSimple) {}

FlashTransactionWrite::FlashTransactionWrite(
    TransactionSourceType source, StreamID stream_id,
    unsigned int data_size_in_byte, LPA logical_page_address,
    ssd_components::UserRequest* related_user_request,
    IOFlowPriorityClass::Priority priority_class, MemoryContent content,
    PageStatus write_sectors_bitmap, DataTimeStamp data_time_stamp)
    : FlashTransaction(stream_id, source, TransactionType::kWrite,
                       related_user_request, priority_class, data_size_in_byte,
                       logical_page_address, NO_PPA),
      content_(content),
      related_read_(NULL),
      write_sectors_bitmap_(write_sectors_bitmap),
      data_time_stamp_(data_time_stamp),
      execution_mode_(WriteExecutionModeType::kSimple) {}

FlashTransactionWrite::FlashTransactionWrite(
    TransactionSourceType source, StreamID stream_id,
    unsigned int data_size_in_byte, LPA logical_page_address,
    ssd_components::UserRequest* related_user_request, MemoryContent content,
    PageStatus write_sectors_bitmap, DataTimeStamp data_time_stamp)
    : FlashTransaction(stream_id, source, TransactionType::kWrite,
                       related_user_request, IOFlowPriorityClass::kUndefined,
                       data_size_in_byte, logical_page_address, NO_PPA),
      content_(content),
      related_read_(NULL),
      write_sectors_bitmap_(write_sectors_bitmap),
      data_time_stamp_(data_time_stamp),
      execution_mode_(WriteExecutionModeType::kSimple) {}

}  // namespace ssd_components