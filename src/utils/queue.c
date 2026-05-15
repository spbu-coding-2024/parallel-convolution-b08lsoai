#include <stdio.h>
#include <string.h>

#include "queue.h"

queue_t *queue_create(size_t capacity) {
  queue_t *queue = malloc(sizeof(queue_t));
  if (!queue) {
    fprintf(stderr, "Error: malloc failed in queue create\n");
    return NULL;
  }

  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;
  queue->capacity = capacity;

  pthread_mutex_init(&queue->lock, NULL);
  pthread_cond_init(&queue->cond_non_empty, NULL);
  pthread_cond_init(&queue->cond_non_full, NULL);

  return queue;
}

void queue_free(queue_t *queue) {
  if (!queue)
    return;

  while (queue->head) {
    queue_node_t *tmp_node = queue->head;
    queue->head = queue->head->next;
    img_info_free(tmp_node->image);
    free(tmp_node);
  }

  pthread_mutex_destroy(&queue->lock);
  pthread_cond_destroy(&queue->cond_non_empty);
  pthread_cond_destroy(&queue->cond_non_full);
  free(queue);
}

void img_info_free(img_info_t *img_info) {
  if (img_info) {
    free((void *)img_info->filename);
  }
  free(img_info);
}

void queue_enqueue(queue_t *queue, image_t *image, const char *filename) {
  img_info_t *img_info = malloc(sizeof(img_info_t));
  if (!img_info) {
    fprintf(stderr, "Error: malloc failed in enqueue 1\n");
    return;
  }
  img_info->image = image;

  if (filename == NULL) {
    img_info->filename = NULL;
  } else {
    img_info->filename = strdup(filename);
    if (!img_info->filename) {
      fprintf(stderr, "Error: strdup failed\n");
      img_info_free(img_info);
      return;
    }
  }
  
  queue_node_t *node = malloc(sizeof(queue_node_t));
  if (!node) {
    fprintf(stderr, "Error: malloc failed in enqueue 3\n");
    img_info_free(img_info);
    return;
  }
  node->image = img_info;
  node->next = NULL;

  pthread_mutex_lock(&queue->lock);

  while (queue->size >= queue->capacity) {
    pthread_cond_wait(&queue->cond_non_full, &queue->lock);
  }

  if (queue->tail) {
    queue->tail->next = node;
  } else {
    queue->head = node;
  }
  queue->tail = node;
  queue->size++;

  pthread_cond_signal(&queue->cond_non_empty);
  pthread_mutex_unlock(&queue->lock);
}

img_info_t *queue_dequeue(queue_t *queue) {
  pthread_mutex_lock(&queue->lock);

  while (queue->size == 0) {
    pthread_cond_wait(&queue->cond_non_empty, &queue->lock);
  }

  queue_node_t *node = queue->head;
  img_info_t *data = node->image;
  queue->head = queue->head->next;

  if (!queue->head) {
    queue->tail = NULL;
  }
  queue->size--;

  pthread_cond_signal(&queue->cond_non_full);
  pthread_mutex_unlock(&queue->lock);

  free(node);
  return data;
}