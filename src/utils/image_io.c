#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>

#include "image_io.h"
#include "stb_image.h"
#include "stb_image_write.h"

image_t *load_image(const char *filename) {
  if (!filename) {
    fprintf(stderr, "Error: NULL filename provided\n");
    return NULL;
  }

  int width, height, channels;
  unsigned char *image_data =
      stbi_load(filename, &width, &height, &channels, 0);
  if (!image_data) {
    fprintf(stderr, "Error: Could not load image '%s': %s\n", filename,
            stbi_failure_reason());
    return NULL;
  }

  // Currently only RGB is supported
  if (channels != 3) {
    fprintf(stderr, "Error: unsupported number of channels (%d)\n", channels);
    stbi_image_free(image_data);
    return NULL;
  }

  image_t *image = (image_t *)malloc(sizeof(image_t));
  if (!image) {
    fprintf(stderr, "Error: memory allocation failed for image structure.\n");
    stbi_image_free(image_data);
    return NULL;
  }

  image->height = height;
  image->width = width;
  image->channels = channels;
  image->image_data = image_data;

  return image;
}

int store_image(const char *filename, image_t *image) {
  if (!filename) {
    fprintf(stderr, "Error: NULL filename provided\n");
    return -1;
  }

  if (!image || !image->image_data) {
    fprintf(stderr, "Error: NULL pointer in image\n");
    return -1;
  }

  const char *dot = strrchr(filename, '.');
  if (!dot) {
    fprintf(stderr, "Error: no file extension in '%s'\n", filename);
    return -1;
  }

  int result = 0;
  const char *ext = dot + 1;

  if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) {
    result = stbi_write_jpg(filename, image->width, image->height,
                            image->channels, image->image_data, 95);
  } else if (strcmp(ext, "png") == 0) {
    result =
        stbi_write_png(filename, image->width, image->height, image->channels,
                       image->image_data, image->width * image->channels);
  } else if (strcmp(ext, "bmp") == 0) {
    result = stbi_write_bmp(filename, image->width, image->height,
                            image->channels, image->image_data);
  } else if (strcmp(ext, "tga") == 0) {
    result = stbi_write_tga(filename, image->width, image->height,
                            image->channels, image->image_data);
  } else {
    fprintf(stderr, "Error: unsupported format '%s'\n", ext);
    return -1;
  }

  if (result == 0) {
    fprintf(stderr, "Error: could not save image '%s'\n", filename);
    return -1;
  }
  return 0;
}

image_t *create_empty_image(int height, int width, int channels) {
  size_t image_size = width * height * channels;
  unsigned char *new_image_data =
      (unsigned char *)calloc(image_size, sizeof(unsigned char));
  if (!new_image_data) {
    fprintf(stderr, "Error: memory allocation failed for image data.\n");
    return NULL;
  }

  image_t *new_image = (image_t *)malloc(sizeof(image_t));
  if (!new_image) {
    fprintf(stderr, "Error: memory allocation failed for image structure.\n");
    free(new_image_data);
    return NULL;
  }

  new_image->height = height;
  new_image->width = width;
  new_image->channels = channels;
  new_image->image_data = new_image_data;
  return new_image;
}

const char *generate_filename(const char *filename, const char *filter_name) {
  static char output[512];

  if (!filename || !filter_name) {
    fprintf(stderr, "Error: NULL pointer in generate_filename\n");
    return NULL;
  }

  const char *dot = strrchr(filename, '.');
  if (!dot) {
    fprintf(stderr, "Error: invalid filename '%s': missing file extension\n",
            filename);
    return NULL;
  }

  int len = dot - filename;
  if (len >= (int)sizeof(output)) {
    fprintf(stderr, "Error: filename too long\n");
    return NULL;
  }

  char name_no_ext[256];
  if (len >= (int)sizeof(name_no_ext)) {
    len = sizeof(name_no_ext) - 1;
  }
  memcpy(name_no_ext, filename, len);
  name_no_ext[len] = '\0';

  snprintf(output, sizeof(output), "%s_%s%s", name_no_ext, filter_name, dot);

  return output;
}

void free_image(image_t *image) {
  if (image) {
    if (image->image_data) {
      stbi_image_free(image->image_data);
    }
    free(image);
  }
}
