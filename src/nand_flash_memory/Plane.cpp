#include "Plane.h"

namespace nand_flash_memory {
Plane::Plane(unsigned int blocks_per_plane, unsigned int pages_per_block) {
  num_block_ = blocks_per_plane;
  blocks_ = new Block*[blocks_per_plane];
  for (unsigned int i = 0; i < blocks_per_plane; i++) {
    blocks_[i] = new Block(pages_per_block, i);
  }
}

Plane::~Plane() {
  for (unsigned int i = 0; i < num_block_; i++) {
    delete blocks_[i];
  }
  delete blocks_;
}
}  // namespace nand_flash_memory