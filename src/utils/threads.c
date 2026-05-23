#include <pthread.h>
#include <stdio.h>

#include "algo/convolution.h"
#include "threads.h"

void *reader_thread(void *arg) {
  params_t *params = (params_t *)arg;
  queue_t *load_queue = params->load_queue;
  char **files_list = params->filenames;
  size_t files_number = params->images_number;

  while (1) {
    size_t idx = atomic_fetch_add_explicit(params->read_counter, 1,
                                           memory_order_relaxed);
    if (idx >= files_number) {
      atomic_fetch_sub_explicit(params->read_counter, 1, memory_order_relaxed);
      break;
    }
    const char *filename = files_list[idx];
    image_t *src_image = load_image(filename);
    if (!src_image) {
      break;
    }

    queue_enqueue(load_queue, src_image, filename);
  }
  return NULL;
}

void *worker_thread(void *arg) {
  params_t *params = (params_t *)arg;
  queue_t *load_queue = params->load_queue;
  queue_t *save_queue = params->save_queue;
  filter_t *filter = params->filter;
  while (1) {
    img_info_t *img_info = queue_dequeue(load_queue);
    if (!img_info->filename && !img_info->image) {
      img_info_free(img_info);
      break;
    }
    image_t *new_image = NULL;
    switch (params->mode) {
    case MODE_SEQ:
      new_image = seq_convolution(img_info->image, filter);
      break;

    case MODE_ROW:
      new_image = parallel_row_convolution(img_info->image, filter);
      break;

    case MODE_PIXEL:
      new_image = parallel_pixel_convolution(img_info->image, filter);
      break;

    case MODE_COLUMN:
      new_image = parallel_column_convolution(img_info->image, filter);
      break;

    case MODE_BLOCK:
      new_image = parallel_block_convolution(img_info->image, filter);
      break;

    default:
      fprintf(stderr, "Error: unknown mode %d\n", params->mode);
      free_image(new_image);
      free_image(img_info->image);
      img_info_free(img_info);
      break;
    }

    if (!new_image) {
      fprintf(stderr, "Error: convolution failed\n");
      free_image(new_image);
      free_image(img_info->image);
      img_info_free(img_info);
      break;
    }

    queue_enqueue(save_queue, new_image, img_info->filename);

    free_image(img_info->image);
    img_info_free(img_info);
  }
  return NULL;
}

void *writer_thread(void *arg) {
  params_t *params = (params_t *)arg;
  queue_t *save_queue = params->save_queue;
  const char *filter_name = params->filter->name;
  size_t files_number = params->images_number;

  while (1) {

    size_t written = atomic_fetch_add_explicit(params->write_counter, 1,
                                               memory_order_relaxed);

    if (written >= files_number) {
      atomic_fetch_sub_explicit(params->write_counter, 1, memory_order_relaxed);
      break;
    }

    img_info_t *img_info = queue_dequeue(save_queue);
    if (!img_info->filename && !img_info->image) {
      img_info_free(img_info);
      break;
    }
    const char *new_filename =
        generate_filename(img_info->filename, filter_name);
    if (!new_filename) {
      fprintf(stderr, "Error: failed to generate output filename from '%s'\n",
              img_info->filename);
      free_image(img_info->image);
      img_info_free(img_info);
      break;
    }

    int ret = store_image(new_filename, img_info->image);
    if (ret < 0) {
      img_info_free(img_info);
      break;
    }

    free_image(img_info->image);
    img_info_free(img_info);
  }
  return NULL;
}
