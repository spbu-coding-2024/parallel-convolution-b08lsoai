#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filters.h"

// clang-format off
const double motion_blur_arr[81] = {
    1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1};

const double blur_arr[25] = {0, 0, 1, 0, 0,
                             0, 1, 1, 1, 0,
                             1, 1, 1, 1, 1,
                             0, 1, 1, 1, 0,
                             0, 0, 1, 0, 0};

const double gaus_blur_arr[25] = {1, 4, 6, 4, 1,
                                  4, 16, 24, 16,
                                  4, 6, 24, 36, 24,
                                  6, 4, 16, 24, 16,
                                  4, 1, 4, 6, 4, 1};

const double sharpen_arr[25] = {-1, -1, -1, -1, -1,
                                -1, 2, 2, 2, -1,
                                -1, 2, 8, 2, -1,
                                -1, 2, 2, 2, -1,
                                -1, -1, -1, -1, -1};

const double emboss_arr[25] = {-1, -1, -1, -1, 0,
                               -1, -1, -1, 0, 1,
                               -1, -1, 0, 1, 1,
                               -1, 0, 1, 1, 1,
                               0, 1, 1, 1, 1};

const double find_edges_arr[9] = {-1, -1, -1,
                                  -1, 8, -1,
                                  -1, -1, -1};
// clang-format on

filter_t *init_filter(int size, double bias, double factor,
                      const double *filter_arr, const char *name) {
  filter_t *filter = (filter_t *)malloc(sizeof(filter_t));
  if (!filter) {
    fprintf(stderr, "Error: malloc failed for filter structure.\n");
    return NULL;
  }

  filter->filter_arr = (double *)malloc(size * size * sizeof(double));
  if (!filter->filter_arr) {
    fprintf(stderr, "Error: malloc failed for filter array.\n");
    free(filter);
    return NULL;
  }

  filter->size = size;
  filter->bias = bias;
  filter->factor = factor;
  filter->name = name;
  memcpy(filter->filter_arr, filter_arr, size * size * sizeof(double));
  return filter;
}

filter_t *create_filter(filter_name name) {
  switch (name) {
  case (BLUR):
    return init_filter(5, 0.0, 1.0 / 13.0, blur_arr, "blur");
  case (MOTION_BLUR):
    return init_filter(9, 0.0, 1.0 / 9.0, motion_blur_arr, "motion");
  case (GAUSSIAN_BLUR):
    return init_filter(5, 0.0, 1.0 / 256.0, gaus_blur_arr, "gaussian");
  case (FIND_EDGES):
    return init_filter(3, 0.0, 1.0, find_edges_arr, "edges");
  case (EMBOSS):
    return init_filter(5, 128.0, 1.0, emboss_arr, "emboss");
  case (SHARPEN):
    return init_filter(5, 0.0, 1.0 / 8.0, sharpen_arr, "sharpen");
  default:
    fprintf(stderr, "Error: unknown filter.\n");
    return NULL;
  }
}

void free_filter(filter_t *filter) {
  if (filter) {
    if (filter->filter_arr) {
      free(filter->filter_arr);
    }
    free(filter);
  }
}
