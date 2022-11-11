#ifndef ARG_PARSER_H
#define ARG_PARSER_H
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

typedef struct {
  char **files;
  bool print_all;
  int file_count;
  size_t size;
} file_tracker_t;

int8_t parse_args(int argc, const char **argv, file_tracker_t *args);
file_tracker_t *init_tracker();
void free_tracker(file_tracker_t *tracker);

#endif // ARG_PARSER_H
