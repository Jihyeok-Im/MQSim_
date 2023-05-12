#ifndef SIM_DEFINITION_H_
#define SIM_DEFINITION_H_

#include <cstdint>
#include <iostream>
#include <string>

typedef uint64_t SimTime;
typedef uint16_t StreamID;
typedef SimTime DataTimeStamp;
typedef std::string SimObjectID;

#define INVALID_TIME \
  18446744073709551615ULL  // event is created
                           // but its time stamp is not yet available
#define T0 0
#define INVALID_TIME_STAMP \
  18446744073709551615ULL  // event's time stamp is invalid
#define MAXIMUM_TIME 18446744073709551615ULL
#define ONE_SECOND 1000000000
#define CURRENT_TIME_STAMP Simulator->Time()
#define SIM_TIME_TO_MICROSECONDS_COEFF 1000
#define SIM_TIME_TO_SECONDS_COEFF 1000000000

#define PRINT_ERROR(MSG)           \
  {                                \
    std::cerr << "ERROR:";         \
    std::cerr << MSG << std::endl; \
    std::cin.get();                \
    exit(1);                       \
  }
#define PRINT_MESSAGE(M) std::cout << M << std::endl;

#endif