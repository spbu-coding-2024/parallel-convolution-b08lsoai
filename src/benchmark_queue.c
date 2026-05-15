#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algo/process_image.h"

#define NUM_RUNS 10

#define BENCH_EXEC(mode, res)                                                  \
  for (size_t i = 0; i < NUM_RUNS; i++) {                                      \
    double start = omp_get_wtime();                                            \
    ret = mode(args);                                                          \
    double end = omp_get_wtime();                                              \
    res[i] = end - start;                                                      \
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

static void save_results_csv(double *queue_res, double *standart_res) {

  FILE *f = fopen("./benchmark_res/result_5.csv", "w");
  if (f == NULL) {
    perror("Failed to create result file");
    return;
  }

  fprintf(f, "# Benchmark Results\n");
  fprintf(f, "# Runs: %d\n", NUM_RUNS);
  fprintf(f, "#\n");

  fprintf(f, "mode");
  fprintf(f, ",avg");
  for (int i = 1; i <= NUM_RUNS; i++) {
    fprintf(f, ",run_%d", i);
  }
  fprintf(f, "\n");

  fprintf(f, "queue,%.6f", average(queue_res));
  for (int i = 0; i < NUM_RUNS; i++)
    fprintf(f, ",%.6f", queue_res[i]);

  fprintf(f, "\n");

  fprintf(f, "standart,%.6f", average(standart_res));
  for (int i = 0; i < NUM_RUNS; i++)
    fprintf(f, ",%.6f", standart_res[i]);

  fclose(f);
  printf("\nResults saved\n");
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s --filter=<filter> --mode=<mode> <image>\n",
            argv[0]);
    fprintf(stderr, "Warning: The benchmark ignores the --queue flag, running "
                    "both modes\n");
    return -1;
  }

  args_t *args = (args_t *)calloc(1, sizeof(args_t));
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

  double queue_res[NUM_RUNS];
  double standart_res[NUM_RUNS];

  printf("Running pipeline benchmarks...\n\n");
  BENCH_EXEC(process_image_queue, queue_res);
  print_results("Pipeline results:", queue_res);
  printf("\n");

  printf("Running standart benchmarks...\n\n");
  BENCH_EXEC(process_image, standart_res);
  print_results("Standart results:", standart_res);
  printf("\n");

  printf("Average times:\n");
  printf("    Pipeline:.................%.4f\n", average(queue_res));
  printf("    Standart:..................%.4f\n", average(standart_res));

  if (ret < 0) {
    free_args(args);
    return -1;
  }

  save_results_csv(queue_res, standart_res);

  free_args(args);
  return ret;
}