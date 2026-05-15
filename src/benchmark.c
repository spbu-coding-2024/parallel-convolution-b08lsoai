#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algo/convolution.h"
#include "utils/filters.h"
#include "utils/image_io.h"

#define NUM_RUNS 10

#define BENCH_EXEC(mode)                                                       \
  for (size_t i = 0; i < NUM_RUNS; i++) {                                      \
    double start = omp_get_wtime();                                            \
    image_t *new_image = mode(image, filter);                                  \
    double end = omp_get_wtime();                                              \
    res[i] = end - start;                                                      \
    free_image(new_image);                                                     \
  }

double average(double *res) {
  double sum = 0;
  for (size_t i = 0; i < NUM_RUNS; i++)
    sum += res[i];
  return sum / NUM_RUNS;
}

static void print_results(const char *name, double *res) {
  printf("%-8s", name);
  for (int i = 0; i < NUM_RUNS; i++)
    printf("%s%.4f", (i == 0) ? " " : ", ", res[i]);
  printf("\n");
}

static void save_results_csv(image_t *image, char *filter_name, double *seq_res,
                             double *row_res, double *pixel_res,
                             double *column_res, double *block_res) {

  FILE *f = fopen("./benchmark_res/resul1t.csv", "w");
  if (f == NULL) {
    perror("Failed to create result file");
    return;
  }

  fprintf(f, "# Benchmark Results\n");
  fprintf(f, "# Filter: %s\n", filter_name);
  fprintf(f, "# Image: %dx%d, %d channels\n", image->width, image->height,
          image->channels);
  fprintf(f, "# Runs: %d\n", NUM_RUNS);
  fprintf(f, "#\n");

  fprintf(f, "mode");
  for (int i = 1; i <= NUM_RUNS; i++) {
    fprintf(f, ",run_%d", i);
  }
  fprintf(f, "\n");

  fprintf(f, "seq: %.6f", average(seq_res));
  for (int i = 0; i < NUM_RUNS; i++)
    fprintf(f, ",%.6f", seq_res[i]);
  fprintf(f, "\n");

  fprintf(f, "row: %.6f", average(row_res));
  for (int i = 0; i < NUM_RUNS; i++)
    fprintf(f, ",%.6f", row_res[i]);
  fprintf(f, "\n");

  fprintf(f, "pixel: %.6f", average(pixel_res));
  for (int i = 0; i < NUM_RUNS; i++)
    fprintf(f, ",%.6f", pixel_res[i]);
  fprintf(f, "\n");

  fprintf(f, "column: %.6f", average(column_res));
  for (int i = 0; i < NUM_RUNS; i++)
    fprintf(f, ",%.6f", column_res[i]);
  fprintf(f, "\n");

  fprintf(f, "block: %.6f", average(block_res));
  for (int i = 0; i < NUM_RUNS; i++)
    fprintf(f, ",%.6f", block_res[i]);
  fprintf(f, "\n");

  fclose(f);
  printf("\nResults saved\n");
}

void run_benchmark(image_t *image, filter_t *filter, conv_mode mode,
                   double *res) {
  switch (mode) {
  case MODE_SEQ:
    BENCH_EXEC(seq_convolution);
    break;

  case MODE_ROW:
    BENCH_EXEC(parallel_row_convolution);
    break;

  case MODE_PIXEL:
    BENCH_EXEC(parallel_pixel_convolution);
    break;

  case MODE_COLUMN:
    BENCH_EXEC(parallel_column_convolution);
    break;

  case MODE_BLOCK:
    BENCH_EXEC(parallel_block_convolution);
    break;

  default:
    fprintf(stderr, "Error: unknown mode %d\n", mode);
    return;
  }
}

// Usage: make bench FILTER=<filter> IMAGE=<image>
int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Error: filter and image are required\n");
    return -1;
  }

  filter_t *filter = NULL;
  if (strcmp(argv[1], "blur") == 0) {
    filter = create_filter(BLUR);
  } else if (strcmp(argv[1], "motion") == 0) {
    filter = create_filter(MOTION_BLUR);
  } else if (strcmp(argv[1], "gaussian") == 0) {
    filter = create_filter(GAUSSIAN_BLUR);
  } else if (strcmp(argv[1], "edge") == 0) {
    filter = create_filter(FIND_EDGES);
  } else if (strcmp(argv[1], "emboss") == 0) {
    filter = create_filter(EMBOSS);
  } else if (strcmp(argv[1], "sharpen") == 0) {
    filter = create_filter(SHARPEN);
  }

  if (!filter) {
    fprintf(stderr, "Error: failed to create filter\n");
    return -1;
  }

  image_t *image = load_image(argv[2]);
  if (!image) {
    fprintf(stderr, "Error: failed to load image '%s'\n", argv[2]);
    free_filter(filter);
    return -1;
  }

  printf("========================================\n");
  printf("Benchmark Configuration\n");
  printf("========================================\n");
  printf("Image:       %s (%dx%d, %d channels)\n", argv[2], image->width,
         image->height, image->channels);
  printf("Filter:      %s (size: %d)\n", argv[1], filter->size);
  printf("Runs:        %d\n", NUM_RUNS);
  printf("========================================\n\n");

  double seq_res[NUM_RUNS];
  double pixel_res[NUM_RUNS];
  double row_res[NUM_RUNS];
  double column_res[NUM_RUNS];
  double block_res[NUM_RUNS];

  printf("Running benchmarks...\n\n");

  run_benchmark(image, filter, MODE_SEQ, seq_res);
  run_benchmark(image, filter, MODE_ROW, row_res);
  run_benchmark(image, filter, MODE_PIXEL, pixel_res);
  run_benchmark(image, filter, MODE_COLUMN, column_res);
  run_benchmark(image, filter, MODE_BLOCK, block_res);

  printf("Results (seconds per run):\n");
  printf("----------------------------------------\n");
  print_results("Seq:", seq_res);
  print_results("Row:", row_res);
  print_results("Pixel:", pixel_res);
  print_results("Column:", column_res);
  print_results("Block:", block_res);
  printf("----------------------------------------\n\n");

  printf("Average times:\n");
  printf("    Seq:.....................%.4f\n", average(seq_res));
  printf("    Pixel:...................%.4f\n", average(pixel_res));
  printf("    Row:.....................%.4f\n", average(row_res));
  printf("    Column:..................%.4f\n", average(column_res));
  printf("    Block:...................%.4f\n", average(block_res));

  save_results_csv(image, argv[1], seq_res, row_res, pixel_res, column_res,
                   block_res);

  free_filter(filter);
  free_image(image);
  return 0;
}