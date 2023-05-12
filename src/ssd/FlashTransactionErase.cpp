#include "FlashTransactionErase.h"

namespace ssd_components {
FlashTransactionErase::FlashTransactionErase(
    TransactionSourceType source, StreamID stream_id,
    const nand_flash_memory::PhysicalPageAddress& address)
    : FlashTransaction(stream_id, source, TransactionType::kErase, NULL,
                       IOFlowPriorityClass::kUndefined, 0, NO_LPA, NO_PPA,
                       address) {}
}  // namespace ssd_components