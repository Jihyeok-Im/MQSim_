#ifndef PLANE_H_
#define PLANE_H_

#include "Block.h"
#include "NANDFlashCommand.h"
#include "NANDFlashDef.h"

namespace nand_flash_memory {
class Plane {
 public:
  Plane(unsigned int blocks_per_plane, unsigned int pages_per_block);
  ~Plane();

  Block* get_block(BlockID block_id) { return blocks_[block_id]; }

 private:
  unsigned int num_block_;
  Block** blocks_;
};
}  // namespace nand_flash_memory

#endif