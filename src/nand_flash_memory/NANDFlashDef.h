#ifndef FLASH_DEF_H_
#define FLASH_DEF_H_

#include <cstdint>

#include "../sim/SimDef.h"

typedef uint64_t PageStatus;
typedef uint32_t ChannelID;
typedef uint32_t PackageID;
typedef uint32_t DieID;
typedef uint32_t PlaneID;
typedef uint32_t BlockID;
typedef uint32_t PageID;
typedef uint64_t LPA;
typedef uint64_t PPA;

enum class MLCTechnology {
  kSingleLevelCell = 1,
  kMultiLevelCell = 2,
  kTripleLevelCell = 3
};

enum class PageType { kSLCLSB, kMLCLSB, kMLCMSB, kTLCLSB, kTLCCSB, kTLCMSB };

#define PPA_TO_PAGE_ID(ppa) ((ppa) & (0x00000000000007FF))                // 11
#define PPA_TO_BLOCK_ID(ppa) (((ppa) >> (11)) & (0x00000000000000FF))     // 8
#define PPA_TO_PLANE_ID(ppa) (((ppa) >> (19)) & (0x0000000000000003))     // 2
#define PPA_TO_DIE_ID(ppa) (((ppa) >> (21)) & (0x0000000000000003))       // 2
#define PPA_TO_PACKAGE_ID(ppa) (((ppa) >> (23)) & (0x00000000000000003))  // 2
#define PPA_TO_CHANNEL_ID(ppa) (((ppa) >> (25)) & (0x000000000000001F))   // 5

#define NO_LPA 0xffffffffffffffffULL
#define NO_STREAM 0xff
#define NO_PPA 0xffffffffffffffffULL
#define NO_MPPN 0xffffffffULL

#endif