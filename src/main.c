#include <stdio.h>
#include <stdlib.h>

#include "algo/process_image.h"

args_t *args = NULL;

int main(int argc, char **argv) {
  args = (args_t *)calloc(1, sizeof(args_t));
  if (!args) {
    fprintf(stderr, "Error: Cannot allocate args structure.\n");
    return -1;
  }

  int ret = parse_args(argc, argv, args);
  if (ret < 0) {
    free_args(args);
    return -1;
  }

  if (args->help_flag) {
    free_args(args);
    return 0;
  }

  if (args->queue_flag) {
    ret = process_image_queue(args);
  } else {
    ret = process_image(args);
  }
  if (ret < 0) {
    free_args(args);
    return -1;
  }
  free_args(args);
  return 0;
}
