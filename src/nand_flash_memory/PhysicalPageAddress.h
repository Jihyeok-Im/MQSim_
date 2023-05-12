#ifndef PHYSICAL_PAGE_ADDRESS_H_
#define PHYSICAL_PAGE_ADDRESS_H_

#include "NANDFlashDef.h"

namespace nand_flash_memory {
class PhysicalPageAddress {
 public:
  ChannelID channel_id_;
  PackageID chip_id_;
  DieID die_id_;
  PlaneID plane_id_;
  BlockID block_id_;
  PageID page_id_;
  PhysicalPageAddress(const ChannelID kChannelID = 0,
                      const PackageID kPackageID = 0, const DieID kDieID = 0,
                      const PlaneID kPlaneID = 0, const BlockID kBlockID = 0,
                      const PageID kPageID = 0) {
    channel_id_ = kChannelID;
    chip_id_ = kPackageID;
    die_id_ = kDieID;
    plane_id_ = kPlaneID;
    block_id_ = kBlockID;
    page_id_ = kPageID;
  }
  PhysicalPageAddress(const PhysicalPageAddress& kAddressToCopy) {
    channel_id_ = kAddressToCopy.channel_id_;
    chip_id_ = kAddressToCopy.chip_id_;
    die_id_ = kAddressToCopy.die_id_;
    plane_id_ = kAddressToCopy.plane_id_;
    block_id_ = kAddressToCopy.block_id_;
    page_id_ = kAddressToCopy.page_id_;
  }

 private:
  // static bool block_address_constraint_for_multiplane_;
};
}  // namespace nand_flash_memory

#endif