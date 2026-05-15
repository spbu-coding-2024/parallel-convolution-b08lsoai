#pragma once
#include "parser_args.h"

typedef struct {
  int size;
  double bias;
  double factor;
  double *filter_arr;
  const char *name;
} filter_t;

filter_t *create_filter(filter_name name);
void free_filter(filter_t *filter);
