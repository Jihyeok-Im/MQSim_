#ifndef HOST_INTERFACE_H_
#define HOST_INTERFACE_H_

#include <stdint.h>

#include <cstring>
#include <vector>

#include "../host/PCIeMessage.h"
#include "../host/PCIeSwitch.h"
#include "../sim/SimObject.h"
#include "../sim/SimReporter.h"
#include "DataCacheManagerBase.h"
#include "UserRequest.h"

namespace host_components {
class PCIeSwtich;
}

namespace ssd_components {
#define COPYDATA(DEST, SRC) DEST = SRC;
#define DELETE_REQUEST_NVME(REQ)                                              \
  delete (Submission_Queue_Entry*)REQ->IO_command_info;                       \
  if (REQ->Transaction_list.size() != 0)                                      \
    PRINT_ERROR("Deleting an unhandled user requests in the host interface!") \
  delete REQ;

class DataCacheManagerBase;
class HostInterface;

class InputStream {
 public:
  InputStream();
  virtual ~InputStream();

 private:
  unsigned int stat_no_of_read_requests_;
  unsigned int stat_no_of_write_request_;
  unsigned int stat_no_of_read_transactions_;
  unsigned int stat_no_of_write_transactions_;
  SimTime stat_sum_of_read_transactions_execution_time_,
      stat_sum_of_read_transactions_transfer_time_,
      stat_sum_of_read_transactions_waiting_time_;
  SimTime stat_sum_of_write_transactions_execution_time_,
      stat_sum_of_write_transactions_transfer_time_,
      stat_sum_of_write_transactions_waiting_time_;
};

class InputStreamManager {
  friend class RequestFetchUnit;

 public:
  InputStreamManager(HostInterface* host_interface, uint16_t queue_fetch_size);
  unsigned int queue_fetch_size_;
  StreamID CreateNewStream(IOFlowPriorityClass::Priority priority_class,
                           LogicalHostAddressType start_logical_sector_address,
                           LogicalHostAddressType end_logical_sector_address,
                           uint64_t submission_queue_base_address,
                           uint16_t submission_queue_size,
                           uint64_t completion_queue_base_address,
                           uint16_t completion_queue_size);
  void SubmissionQueueTailPointerUpdate(StreamID stream_id,
                                        uint16_t tail_pointer);
  void CompletionQueueHeadPointerUpdate(StreamID stream_id,
                                        uint16_t head_pointer);
  uint16_t get_submission_queue_depth(StreamID stream_id);
  uint16_t get_completion_queue_depth(StreamID stream_id);
  IOFlowPriorityClass::Priority get_priority_class(StreamID stream_id);
  ~InputStreamManager();
  void HandleNewArrivedRequest(UserRequest* request);
  void HandleArrivedWriteData(UserRequest* request);
  void HandleServicedRequest(UserRequest* request);
  void UpdateTransactionStatistics(FlashTransaction* transaction);
  uint32_t get_average_read_transaction_turnaround_time(StreamID stream_id);
  uint32_t get_average_read_transaction_execution_time(StreamID streamd_id);
  uint32_t get_average_read_transaction_transfer_time(StreamID streamd_id);
  uint32_t get_average_read_transaction_waiting_time(StreamID streamd_id);
  uint32_t get_average_write_transaction_turnaround_time(StreamID stream_id);
  uint32_t get_average_write_transaction_execution_time(StreamID stream_id);
  uint32_t get_average_write_transaction_transfer_time(StreamID stream_id);
  uint32_t get_average_write_transaction_waiting_time(StreamID stream_id);

 private:
  void SegmentUserRequest(UserRequest* user_request);
  void InformHostRequestCompleted(StreamID stream_id, UserRequest* request);
  std::vector<InputStream*> input_streams_;
};

class RequestFetchUnit {
 public:
  RequestFetchUnit(HostInterface* host_interface);
  ~RequestFetchUnit();
  void FetchNextRequest(StreamID stream_id);
  void FetchWriteData(UserRequest* request);
  void SendReadData(UserRequest* request);
  void SendCompletionQueueElement(UserRequest* request,
                                  uint16_t submission_queue_head_pointer);
  void ProcessPCIeWriteMessage(uint64_t, void*, unsigned int);
  void ProcessPCIeReadMessage(uint64_t, void*, unsigned int);

 protected:
  enum class DirectMemoryAccessRequireType { kRequestInfo, kWriteData };
  struct DirectMemoryAccessRequireItem {
    DirectMemoryAccessRequireType type;
    void* object;
  };
  HostInterface* host_interface_;
  std::list<DirectMemoryAccessRequireItem*> direct_memory_access_list_;

 private:
  uint16_t current_phase_;
  uint32_t no_of_sent_completion_queue_entry_;
};

class HostInterface : public sim_engine::SimObject,
                      public sim_engine::SimReporter {
  friend class InputStreamManager;
  friend class RequestFetchUnit;
  typedef void (*UserRequestArrivedSignalHandlerType)(UserRequest*);

 public:
  HostInterface(const SimObjectID& id,
                LogicalHostAddressType max_logical_sector_address,
                uint16_t submission_queue_depth,
                uint16_t completion_queue_depth,
                unsigned int no_of_input_streams, uint16_t queue_fetch_size,
                unsigned int sector_per_page, DataCacheManagerBase* cache);
  StreamID CreateNewStream(IOFlowPriorityClass::Priority priortiy_class,
                           LogicalHostAddressType start_logical_sector_address,
                           LogicalHostAddressType end_logical_sector_address,
                           uint64_t submission_queue_base_address,
                           uint64_t completion_queue_base_address);

  ~HostInterface();
  void SetupTriggers();
  void StartSimulation();
  void ValidateSimulationConfig();
  void ExecuteSimulatorEvent(sim_engine::SimEvent*);
  uint16_t get_submission_queue_depth();
  uint16_t get_completion_queue_depth();
  //   void Report_results_in_XML(std::string name_prefix,
  //                              Utils::XmlWriter& xmlwriter);
  void ConnectToUserRequestArrivedSignal(
      UserRequestArrivedSignalHandlerType function) {
    connected_user_request_arrived_signal_handlers_.push_back(function);
  }

  void ConsumePCIeMessage(host_components::PCIeMessage* message) {
    if (message->get_message_type() ==
        host_components::PCIeMessageType::kReadComplete) {
      request_fetch_unit_->ProcessPCIeReadMessage(message->get_address(),
                                                  message->get_payload(),
                                                  message->get_payload_size());
    } else {
      request_fetch_unit_->ProcessPCIeWriteMessage(message->get_address(),
                                                   message->get_payload(),
                                                   message->get_payload_size());
    }
    delete message;
  }

  void SendReadMessageToHost(uint64_t address,
                             unsigned int request_read_data_size);
  void SendWriteMessageToHost(uint64_t address, void* message,
                              unsigned int message_size);

 protected:
  LogicalHostAddressType max_logical_sector_address_;
  unsigned int sectors_per_page_;
  static HostInterface* my_instance_;
  InputStreamManager* input_stream_manager_;
  RequestFetchUnit* request_fetch_unit_;
  DataCacheManagerBase* cache_;
  std::vector<UserRequestArrivedSignalHandlerType>
      connected_user_request_arrived_signal_handlers_;

  void BroadcastUserRequestArrivalSignal(UserRequest* user_request) {
    for (std::vector<UserRequestArrivedSignalHandlerType>::iterator it =
             connected_user_request_arrived_signal_handlers_.begin();
         it != connected_user_request_arrived_signal_handlers_.end(); it++) {
      (*it)(user_request);
    }
  }
  static void HandleUserRequestServicedSignalFromCache(
      UserRequest* user_request) {
    my_instance_->input_stream_manager_->HandleServicedRequest(user_request);
  }

  static void HandleUserMemoryTransactionServicedSignalFromCache(
      FlashTransaction* transaction) {
    my_instance_->input_stream_manager_->UpdateTransactionStatistics(
        transaction);
  }

 private:
  host_components::PCIeSwitch* pcie_switch_;
  uint16_t submission_queue_depth_, completion_queue_depth_;
  unsigned int no_of_input_streams_;
};
}  // namespace ssd_components

#endif