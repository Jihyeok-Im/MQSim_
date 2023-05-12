#include "Block.h"

namespace nand_flash_memory {
Block::Block(unsigned int pages_per_block, BlockID block_id) {
  id_ = block_id;
  pages_ = new Page[pages_per_block];
}

Block::~Block() { delete[] pages_; }
}  // namespace nand_flash_memory
