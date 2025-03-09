#ifndef OPTION_H
#define OPTION_H

#include <stdbool.h>
#include <stdint.h>

// Macro to define an Option type for whatever T type
#define DEFINE_OPTION_TYPE(T, name)			\
  typedef struct {					\
    T value;						\
    bool has_value;					\
  } name;						\
							\
  static inline name some_##name(T value) {		\
    return (name){ .value = value, .has_value = true }; \
  }							\
							\
  static inline name none_##name() {			\
    return (name){ .has_value = false };		\
  }

DEFINE_OPTION_TYPE(int, Option_int)
DEFINE_OPTION_TYPE(unsigned int, Option_uint)
DEFINE_OPTION_TYPE(short, Option_short)
DEFINE_OPTION_TYPE(unsigned short, Option_ushort)
DEFINE_OPTION_TYPE(long, Option_long)
DEFINE_OPTION_TYPE(unsigned long, Option_ulong)
DEFINE_OPTION_TYPE(long long, Option_long_long)
DEFINE_OPTION_TYPE(unsigned long long, Option_ulong_long)
DEFINE_OPTION_TYPE(float, Option_float)
DEFINE_OPTION_TYPE(double, Option_double)
DEFINE_OPTION_TYPE(long double, Option_long_double)
DEFINE_OPTION_TYPE(char, Option_char)
DEFINE_OPTION_TYPE(unsigned char, Option_uchar)
DEFINE_OPTION_TYPE(int8_t, Option_int8)
DEFINE_OPTION_TYPE(uint8_t, Option_uint8)
DEFINE_OPTION_TYPE(int16_t, Option_int16)
DEFINE_OPTION_TYPE(uint16_t, Option_uint16)
DEFINE_OPTION_TYPE(int32_t, Option_int32)
DEFINE_OPTION_TYPE(uint32_t, Option_uint32)
DEFINE_OPTION_TYPE(int64_t, Option_int64)
DEFINE_OPTION_TYPE(uint64_t, Option_uint64)

#endif // OPTION_H
