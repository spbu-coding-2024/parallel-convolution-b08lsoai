#pragma once

#include "utils/filters.h"
#include "utils/image_io.h"

image_t *seq_convolution(image_t *image, filter_t *filter);
