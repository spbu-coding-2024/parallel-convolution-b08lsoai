#include <stdio.h>

#include "convolution.h"
#include "utils/threads.h"

#define FREE_IMAGES                                                            \
  free_image(image);                                                           \
  free_image(new_image)

int process_image(args_t *args) {
  if (!args) {
    fprintf(stderr, "Error: args are NULL\n");
    return -1;
  }

  filter_t *filter = create_filter(args->filter);
  if (!filter) {
    fprintf(stderr, "Error: failed to create filter\n");
    return -1;
  }

  const char *filter_name = filter->name;
  conv_mode mode = args->mode;

  for (size_t i = 0; i < args->images_number; i++) {
    const char *filename = args->filenames[i];
    image_t *image = NULL;
    image_t *new_image = NULL;

    image = load_image(filename);
    if (!image) {
      free_filter(filter);
      return -1;
    }

    switch (mode) {
    case MODE_SEQ:
      new_image = seq_convolution(image, filter);
      break;

    case MODE_ROW:
      new_image = parallel_row_convolution(image, filter);
      break;

    case MODE_PIXEL:
      new_image = parallel_pixel_convolution(image, filter);
      break;

    case MODE_COLUMN:
      new_image = parallel_column_convolution(image, filter);
      break;

    case MODE_BLOCK:
      new_image = parallel_block_convolution(image, filter);
      break;

    default:
      fprintf(stderr, "Error: unknown mode %d\n", mode);
      FREE_IMAGES;
      free_filter(filter);
      return -1;
    }

    if (!new_image) {
      fprintf(stderr, "Error: convolution failed\n");
      FREE_IMAGES;
      free_filter(filter);
      return -1;
    }

    const char *new_filename = generate_filename(filename, filter_name);
    if (!new_filename) {
      fprintf(stderr, "Error: failed to generate output filename\n");
      FREE_IMAGES;
      free_filter(filter);
      return -1;
    }

    int ret = store_image(new_filename, new_image);
    if (ret < 0) {
      FREE_IMAGES;
      free_filter(filter);
      return -1;
    }

    FREE_IMAGES;
  }

  free_filter(filter);
  return 0;
}

int process_image_queue(args_t *args) {
  if (!args) {
    fprintf(stderr, "Error: args are NULL\n");
    return -1;
  }

  filter_t *filter = create_filter(args->filter);
  if (!filter) {
    fprintf(stderr, "Error: failed to create filter\n");
    return -1;
  }

  queue_t *load_queue = queue_create((size_t)LOAD_QUEUE_SIZE);
  queue_t *save_queue = queue_create((size_t)SAVE_QUEUE_SIZE);
  atomic_size_t read_counter = 0;
  atomic_size_t write_counter = 0;

  params_t params = {.load_queue = load_queue,
                     .save_queue = save_queue,
                     .filter = filter,
                     .filenames = args->filenames,
                     .mode = args->mode,
                     .images_number = args->images_number,
                     .read_counter = &read_counter,
                     .write_counter = &write_counter};

  pthread_t readers[READERS_NUMBER];
  pthread_t workers[WORKERS_NUMBER];
  pthread_t writers[WRITERS_NUMBER];

  for (int i = 0; i < READERS_NUMBER; i++) {
    pthread_create(&readers[i], NULL, reader_thread, &params);
  }

  for (int i = 0; i < WORKERS_NUMBER; i++) {
    pthread_create(&workers[i], NULL, worker_thread, &params);
  }

  for (int i = 0; i < WRITERS_NUMBER; i++) {
    pthread_create(&writers[i], NULL, writer_thread, &params);
  }

  for (int i = 0; i < READERS_NUMBER; i++) {
    pthread_join(readers[i], NULL);
  }

  for (int i = 0; i < WORKERS_NUMBER; i++) {
    queue_enqueue(load_queue, NULL, NULL);
  }

  for (int i = 0; i < WORKERS_NUMBER; i++) {
    pthread_join(workers[i], NULL);
  }

  for (int i = 0; i < WRITERS_NUMBER; i++) {
    queue_enqueue(save_queue, NULL, NULL);
  }

  for (int i = 0; i < WRITERS_NUMBER; i++) {
    pthread_join(writers[i], NULL);
  }
  queue_free(load_queue);
  queue_free(save_queue);
  free_filter(filter);
  return 0;
}
