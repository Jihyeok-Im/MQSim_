#ifndef SIM_REPORTER_H_
#define SIM_REPORTER_H_

#include <string>

#include "../utils/XMLWriter.h"

namespace sim_engine {
class SimReporter {
 public:
  virtual void ReportResultsInXml(std::string name_prefix,
                                  Utils::XmlWriter &xml_writer) = 0;

  // host system, FTL, IO Flow(req count, 등), Flash Transaction
  //  Queue(#of transaction, ...), TSU(Mapping Read TR Queue, ...)
};
}  // namespace sim_engine

#endif