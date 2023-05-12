#ifndef FLASH_TRANSACTION_ERASE_H_
#define FLASH_TRANSACTION_ERASE_H_

#include <list>

#include "../nand_flash_memory/NANDFlashDef.h"
#include "FlashTransaction.h"
#include "FlashTransactionWrite.h"

namespace ssd_components {
class FlashTransactionErase : public FlashTransaction {
 public:
  FlashTransactionErase(TransactionSourceType source, StreamID stream_id,
                        const nand_flash_memory::PhysicalPageAddress& address);
  std::list<FlashTransactionWrite*> page_movement_activities;
};
}  // namespace ssd_components

#endif