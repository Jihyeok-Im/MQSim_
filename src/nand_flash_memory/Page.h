#ifndef PAGE_H_
#define PAGE_H_

#include "NANDFlashDef.h"

namespace nand_flash_memory {
typedef uint64_t UserData;
struct OOBData {
  LPA lpa;
};

class Page {
 public:
  Page() { oob_data_.lpa = NO_LPA; };
  void Write(const OOBData& kMetadata, const UserData& kUserData) {
    this->oob_data_.lpa = kMetadata.lpa;
    this->user_data_ = kUserData;
  }

  void Read(OOBData& metadata, UserData& user_data) {
    metadata.lpa = this->oob_data_.lpa;
    user_data = this->user_data_;
  }
  void set_lpa(LPA lpa) { this->oob_data_.lpa = lpa; }
  void set_data(uint64_t data) { this->user_data_ = data; }

 private:
  OOBData oob_data_;
  UserData user_data_;
};
}  // namespace nand_flash_memory

#endif
