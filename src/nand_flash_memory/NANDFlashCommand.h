#ifndef FLASH_COMMAND_H_
#define FLASH_COMMAND_H_

#include <vector>

#include "NANDFlashDef.h"
#include "Page.h"
#include "PhysicalPageAddress.h"

enum class NANDFlashCommandCode {
  kPageRead,
  kPageReadMultiplane,
  kPageReadCopyback,
  kPageReadCopybackMultiplane,
  kPageProg,
  kPageProgMultiplane,
  kPageProgCopyback,
  kPageProgCopybackMultiplane,
  kBlockErase,
  kBlockEraseMulitplane
};

namespace nand_flash_memory {
class NANDFlashCommand {
 public:
  NANDFlashCommandCode get_command_code() { return command_code_; }
  std::vector<PPA> get_address() { return address_; }
  std::vector<OOBData> get_oob_data() { return oob_data_; }
  std::vector<UserData> get_user_data() { return user_data_; }

 private:
  NANDFlashCommandCode command_code_;
  std::vector<PPA> address_;
  std::vector<OOBData> oob_data_;
  std::vector<UserData> user_data_;
};
}  // namespace nand_flash_memory

#endif