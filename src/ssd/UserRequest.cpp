#include "UserRequest.h"

namespace ssd_components {
unsigned int UserRequest::lastID = 0;

UserRequest::UserRequest() : sectors_serviced_from_cache_(0) {
  id_ = "" + std::to_string(lastID++);
  is_ignored_ = false;
}
}  // namespace ssd_components