#ifndef BLOCK_H_
#define BLOCK_H_

#include "NANDFlashDef.h"
#include "Page.h"

namespace nand_flash_memory {
class Block {
 public:
  Block(unsigned int pages_per_block, BlockID block_id);
  ~Block();

  BlockID get_block_id() { return id_; }
  Page get_page(PageID page_id) { return pages_[page_id]; }

 private:
  BlockID id_;
  Page* pages_;
  MLCTechnology current_technology_;
};
}  // namespace nand_flash_memory

#endif