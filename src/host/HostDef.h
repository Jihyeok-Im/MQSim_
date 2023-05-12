#ifndef HOST_DEF_H_
#define HOST_DEF_H_

#include <cstdint>

#define QUEUE_ID_TO_FLOW_ID(Q) Q - 1
#define FLOW_ID_TO_Q_ID(F) F + 1
#define NVME_COMP_Q_MEMORY_REGION 40
#define DATA_MEMORY_REGION 0xFF0000000000

// io quque id 1~8
const uint64_t kSubmissionQueueMemory0 = 0x000000000000;
const uint64_t kCompletionQueueMemory0 = 0x00F000000000;
const uint64_t kSubmissionQueueMemory1 = 0x010000000000;
const uint64_t kCompletionQueueMemory1 = 0x01F000000000;
const uint64_t kSubmissionQueueMemory2 = 0x020000000000;
const uint64_t kCompletionQueueMemory2 = 0x02F000000000;
const uint64_t kSubmissionQueueMemory3 = 0x030000000000;
const uint64_t kCompletionQueueMemory3 = 0x03F000000000;
const uint64_t kSubmissionQueueMemory4 = 0x040000000000;
const uint64_t kCompletionQueueMemory4 = 0x04F000000000;
const uint64_t kSubmissionQueueMemory5 = 0x050000000000;
const uint64_t kCompletionQueueMemory5 = 0x05F000000000;
const uint64_t kSubmissionQueueMemory6 = 0x060000000000;
const uint64_t kCompletionQueueMemory6 = 0x06F000000000;
const uint64_t kSubmissionQueueMemory7 = 0x070000000000;
const uint64_t kCompletionQueueMemory7 = 0x07F000000000;
const uint64_t kSubmissionQueueMemory8 = 0x080000000000;
const uint64_t kCompletionQueueMemory8 = 0x08F000000000;

#endif  //! HOST_DEF_H_
