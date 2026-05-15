#pragma once

#include <pthread.h>
#include <stdlib.h>

#include "image_io.h"

#define LOAD_QUEUE_SIZE 10
#define SAVE_QUEUE_SIZE 10

// ?: add id for pile
// TODO: rename to task_data_t ?
typedef struct {
  image_t *image;
  const char *filename;
} img_info_t;

typedef struct queue_node {
  struct queue_node *next;
  img_info_t *image;
} queue_node_t;

// TODO: rename mutex
typedef struct {
  queue_node_t *head;
  queue_node_t *tail;
  size_t size;
  size_t capacity;
  pthread_mutex_t lock;
  pthread_cond_t cond_non_empty;
  pthread_cond_t cond_non_full;
} queue_t;

queue_t *queue_create(size_t capacity);
void queue_enqueue(queue_t *queue, image_t *image, const char *filename);
img_info_t *queue_dequeue(queue_t *queue);
void queue_free(queue_t *queue);
void img_info_free(img_info_t *img_info);