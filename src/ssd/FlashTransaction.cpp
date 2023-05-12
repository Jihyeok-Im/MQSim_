#include "FlashTransaction.h"

namespace ssd_components {
FlashTransaction::FlashTransaction(
    StreamID stream_id, TransactionSourceType source, TransactionType type,
    UserRequest* user_request, IOFlowPriorityClass::Priority priority_class,
    unsigned int data_size_in_byte, LPA logical_page_address,
    PPA physical_page_address)
    : stream_id_(stream_id),
      source_(source),
      type_(type),
      user_request_(user_request),
      priority_class_(priority_class),
      issue_time_(Simulator->Time()),
      stat_execution_time_(INVALID_TIME),
      stat_transfer_time_(INVALID_TIME),
      data_and_metadata_size_in_byte_(data_size_in_byte),
      logical_page_address_(logical_page_address),
      physical_page_address_(physical_page_address),
      physical_address_determined_(false) {}

FlashTransaction::FlashTransaction(
    StreamID stream_id, TransactionSourceType source, TransactionType type,
    UserRequest* user_request, IOFlowPriorityClass::Priority priority_class,
    unsigned int data_size_in_byte, LPA logical_page_address,
    PPA physical_page_address,
    const nand_flash_memory::PhysicalPageAddress& address)
    : stream_id_(stream_id),
      source_(source),
      type_(type),
      user_request_(user_request),
      priority_class_(priority_class),
      issue_time_(Simulator->Time()),
      stat_execution_time_(INVALID_TIME),
      stat_transfer_time_(INVALID_TIME),
      data_and_metadata_size_in_byte_(data_size_in_byte),
      logical_page_address_(logical_page_address),
      physical_page_address_(physical_page_address),
      address_(address),
      physical_address_determined_(false) {}

}  // namespace ssd_components