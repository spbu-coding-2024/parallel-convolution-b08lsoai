#pragma once

#include <stdatomic.h>

#include "filters.h"
#include "parser_args.h"
#include "queue.h"

#define READERS_NUMBER 2
#define WORKERS_NUMBER 4
#define WRITERS_NUMBER 2

typedef struct {
  queue_t *load_queue;
  queue_t *save_queue;
  filter_t *filter;
  char **filenames;
  size_t images_number;
  conv_mode mode;
  atomic_size_t *read_counter;
  atomic_size_t *write_counter;
} params_t;

void *reader_thread(void *arg);
void *worker_thread(void *arg);
void *writer_thread(void *arg);
