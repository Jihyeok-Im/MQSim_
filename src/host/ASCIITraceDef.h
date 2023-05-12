#ifndef ASCII_TRACE_DEF_H_
#define ASCII_TRACE_DEF_H_

enum class TraceTimeUnit {
  kPicosecond,
  kNanosecond,
  kMicrosecond
};  // The unit of arrival times in the input file
#define PICOSECOND_COEFFICIENT \
  1000000000000  // the coefficient to convert picoseconds to second
#define NANOSECOND_COEFFICIENT \
  1000000000  // the coefficient to convert nanoseconds to second
#define MICROSECOND_COEFFICIENT \
  1000000  // the coefficient to convert microseconds to second
#define ASCII_TRACE_TIME_COLUMN 0
#define ASCII_TRACE_DEVICE_COLUMN 1
#define ASCII_TRACE_ADDRESS_COLUMN 2
#define ASCII_TRACE_SIZE_COLUMN 3
#define ASCII_TRACE_TYPE_COLUMN 4
#define ASCII_TRACE_WRITE_CODE "0"
#define ASCII_TRACE_READ_CODE "1"
#define ASCII_TRACE_WRITE_CODE_INTEGER 0
#define ASCII_TRACE_READ_CODE_INTEGER 1
#define ASCII_TRACE_LINE_DELIMITER ' '
#define ASCII_TRACE_ITEMS_PER_LINE 5

#endif  // !ASCII_TRACE_DEFINITION_H
