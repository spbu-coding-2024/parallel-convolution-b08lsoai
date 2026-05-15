#pragma once

#include "utils/filters.h"
#include "utils/image_io.h"

image_t *seq_convolution(image_t *image, filter_t *filter);
image_t *parallel_pixel_convolution(image_t *image, filter_t *filter);
image_t *parallel_row_convolution(image_t *image, filter_t *filter);
image_t *parallel_column_convolution(image_t *image, filter_t *filter);
image_t *parallel_block_convolution(image_t *image, filter_t *filter);
