#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser_args.h"

#define INITIAL_CAPACITY 8

#define USAGE_PRINT                                                            \
  fprintf(stderr,                                                              \
          "Usage: %s --filter=<filter_name> --mode=<mode> [--help | -h] "      \
          "[--queue | -q]"                                                     \
          "<input_file>\n",                                                    \
          argv[0]);                                                            \
  fprintf(stderr, "Use '%s --help' for more information.\n", argv[0]);

// Basic check: not empty and has extension
// Actual file existence and format validation are handled by load_image()
bool is_file(char *filename) { return (filename && strchr(filename, '.')); }

void init_args(args_t *args) {
  args->filter = INVALID;
  args->help_flag = false;
  args->queue_flag = false;
  args->filenames = NULL;
  args->images_number = 0;
  args->mode = MODE_INVALID;
}

filter_name parse_filter_arg(char *filter_str) {
  if (strcmp(filter_str, "blur") == 0) {
    return BLUR;
  } else if (strcmp(filter_str, "motion") == 0) {
    return MOTION_BLUR;
  } else if (strcmp(filter_str, "gaussian") == 0) {
    return GAUSSIAN_BLUR;
  } else if (strcmp(filter_str, "edges") == 0) {
    return FIND_EDGES;
  } else if (strcmp(filter_str, "emboss") == 0) {
    return EMBOSS;
  } else if (strcmp(filter_str, "sharpen") == 0) {
    return SHARPEN;
  }
  return INVALID;
}

conv_mode parse_mode_arg(char *mode_str) {
  if (strcmp(mode_str, "seq") == 0) {
    return MODE_SEQ;
  } else if (strcmp(mode_str, "pixel") == 0) {
    return MODE_PIXEL;
  } else if (strcmp(mode_str, "row") == 0) {
    return MODE_ROW;
  } else if (strcmp(mode_str, "column") == 0) {
    return MODE_COLUMN;
  } else if (strcmp(mode_str, "block") == 0) {
    return MODE_BLOCK;
  }
  return MODE_INVALID;
}

static int add_filename(args_t *args, char *filename) {
  static size_t capacity = 0;

  if (args->filenames == NULL) {
    capacity = INITIAL_CAPACITY;
    args->filenames = malloc(capacity * sizeof(char *));
    if (!args->filenames) {
      fprintf(stderr, "Error: memory allocation failed while adding file\n");
      return -1;
    }
  }

  if (args->images_number >= capacity) {
    capacity *= 2;
    char **new = realloc(args->filenames, capacity * sizeof(char *));
    if (!new) {
      fprintf(stderr, "Error: memory allocation failed while adding file\n");
      return -1;
    }
    args->filenames = new;
  }

  args->filenames[args->images_number++] = filename;
  return 0;
}

void print_help(char *argv[]) {
  printf("Usage: %s --filter=<filter_name> --mode=<mode> [--help | -h] "
         "[--queue | -q] "
         "<input_file>\n\n",
         argv[0]);

  printf("Options:\n");
  printf("  --filter=     Filter to apply\n");
  printf("                  Available: blur, motion, gaussian, edges, emboss, "
         "sharpen\n\n");
  printf("  --mode=       Parallelization strategy\n");
  printf("                  seq      - Sequential\n");
  printf("                  row      - Parallel by rows\n");
  printf("                  column   - Parallel by columns\n");
  printf("                  pixel    - Parallel by individual pixels\n");
  printf("                  block    - Parallel by blocks (64x64)\n\n");
  printf("  --queue, -q   Enable pipeline mode (process multiple images)\n\n");
  printf("  --help, -h    Show this help message\n\n");
}

int parse_args(int argc, char *argv[], args_t *args) {
  if (argc < 2) {
    fprintf(stderr, "Error: missing required arguments\n");
    USAGE_PRINT
    return -1;
  }

  if (!args) {
    fprintf(stderr, "Error: args structure cannot be NULL\n");
    return -1;
  }

  init_args(args);

  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--filter=", 9) == 0) {
      char *filter_str = argv[i] + 9;
      if (strlen(filter_str) == 0) {
        fprintf(stderr, "Error: --filter requires a value\n");
        fprintf(
            stderr,
            "Valid filters: blur, motion, gaussian, edges, emboss, sharpen\n");
        return -1;
      }
      args->filter = parse_filter_arg(filter_str);
      if (args->filter == INVALID) {
        fprintf(stderr, "Error: invalid filter '%s'\n", filter_str);
        fprintf(
            stderr,
            "Valid filters: blur, motion, gaussian, edges, emboss, sharpen\n");
        return -1;
      }
    } else if (strncmp(argv[i], "--mode=", 7) == 0) {
      char *mode = argv[i] + 7;
      if (strlen(mode) == 0) {
        fprintf(stderr, "Error: --mode requires a value\n");
        fprintf(stderr, "Valid modes: seq, pixel, row, column, block\n");
        return -1;
      }
      args->mode = parse_mode_arg(mode);
      if (args->mode == MODE_INVALID) {
        fprintf(stderr, "Error: invalid mode '%s'\n", mode);
        fprintf(stderr, "Valid modes: seq, pixel, row, column, block\n");
        return -1;
      }
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      print_help(argv);
      args->help_flag = true;
      return 0;
    } else if (strcmp(argv[i], "--queue") == 0 || strcmp(argv[i], "-q") == 0) {
      args->queue_flag = true;
    } else if (strncmp(argv[i], "-", 1) == 0) {
      fprintf(stderr, "Error: unknown option '%s'\n", argv[i]);
      USAGE_PRINT
      return -1;
    } else {
      if (is_file(argv[i])) {
        if (add_filename(args, argv[i]) < 0) {
          return -1;
        }
      } else {
        fprintf(stderr, "Error: invalid argument '%s'\n", argv[i]);
        USAGE_PRINT
        return -1;
      }
    }
  }

  if (args->filter == INVALID) {
    fprintf(stderr, "Error: --filter is required\n");
    return -1;
  }
  if (args->mode == MODE_INVALID) {
    fprintf(stderr, "Error: --mode is required\n");
    return -1;
  }
  if (args->filenames == NULL || args->images_number == 0) {
    fprintf(stderr, "Error: no input file\n");
    return -1;
  }

  return 0;
}

void free_args(args_t *args) {
  if (args) {
    if (args->filenames) {
      free(args->filenames);
    }
    free(args);
  }
}
