#ifndef JG_NETCALC_SRC_UTILS_UTILS_H_
#define JG_NETCALC_SRC_UTILS_UTILS_H_
#ifdef __cplusplus
extern "C" {
#endif // END __cplusplus

// If not in debug mode, set the debug print to 0. Else enable
#ifdef NDEBUG
#define DEBUG_PRINT 0
#else
#define DEBUG_PRINT 1
#endif
#include <stdio.h>

/*
 * Enable printing debug messages when in debug mode
 */

#define debug_print_err(fmt, ...)                                              \
  do {                                                                         \
    if (DEBUG_PRINT)                                                           \
      fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__,        \
              __VA_ARGS__);                                                    \
  } while (0)

#define debug_print(fmt, ...)                                                  \
  do {                                                                         \
    if (DEBUG_PRINT)                                                           \
      fprintf(stderr, fmt, __VA_ARGS__);                                       \
  } while (0)

typedef enum util_verify_t { UV_VALID_ALLOC, UV_INVALID_ALLOC } util_verify_t;
util_verify_t verify_alloc(void *ptr);

#ifdef __cplusplus
}
#endif // END __cplusplus
#endif // JG_NETCALC_SRC_UTILS_UTILS_H_
