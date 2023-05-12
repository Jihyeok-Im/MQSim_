#ifndef NVME_DEFINITIONS_H
#define NVME_DEFINITIONS_H

#include <cstdint>
#include <string>

#include "HostInterfacePriority.h"

#define NVME_WRITE_OPCODE 0x0001
#define NVME_READ_OPCODE 0x0002

const uint64_t kNCQSubmissionRegister = 0x1000;
const uint64_t kNCQCompletionRegister = 0x1003;
const uint64_t kSubmissionQueueRegister0 = 0x1000;
const uint64_t kCompletionQueueRegister0 = 0x1003;
const uint64_t kSubmissionQueueRegister1 = 0x1010;
const uint64_t kCompletionQueueRegister1 = 0x1013;
const uint64_t kSubmissionQueueRegister2 = 0x1020;
const uint64_t kCompletionQueueRegister2 = 0x1023;
const uint64_t kSubmissionQueueRegister3 = 0x1030;
const uint64_t kCompletionQueueRegister3 = 0x1033;
const uint64_t kSubmissionQueueRegister4 = 0x1040;
const uint64_t kCompletionQueueRegister4 = 0x1043;
const uint64_t kSubmissionQueueRegister5 = 0x1050;
const uint64_t kCompletionQueueRegister5 = 0x1053;
const uint64_t kSubmissionQueueRegister6 = 0x1060;
const uint64_t kCompletionQueueRegister6 = 0x1063;
const uint64_t kSubmissionQueueRegister7 = 0x1070;
const uint64_t kCompletionQueueRegister7 = 0x1073;
const uint64_t kSubmissionQueueRegister8 = 0x1080;
const uint64_t kCompletionQueueRegister8 = 0x1083;

struct CompletionQueueEntry {
  uint32_t command_specific;
  uint32_t reserved;
  uint16_t sq_head_pointer;  // SQ Head Pointer, Indicates the current
                             // Submission Queue Head pointer for the Submission
                             // Queue indicated in the SQ Identifier field
  uint16_t sq_identifier;    // SQ Identifier, Indicates the Submission Queue to
                             // which the associated command was issued to.
  uint16_t command_identifier;  // Command Identifier, Indicates the identifier
                                // of the command that is being completed
  uint16_t
      status_field_and_phase_tag;  // Status Field (SF)+ Phase Tag(P)
                                   // SF: Indicates status for the command that
                                   // is being completed P:Identifies whether a
                                   // Completion Queue entry is new
};

struct SubmissionQueueEntry {
  uint8_t opcode;  // Is it a read or write request
  uint8_t prp_fuse;
  uint16_t
      command_identifier;  // The id of the command in the I/O submission queue
  uint64_t namespace_identifier;
  uint64_t reserved;
  uint64_t metadata_pointer;
  uint64_t prp_entry1;
  uint64_t prp_entry2;
  uint32_t command_specific[6];
};

#endif  // !NVME_DEFINISIONS_H
