// clang-format off
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>

#include "algo/convolution.h"
#include "utils/filters.h"
#include "utils/image_io.h"
// clang-format on

#define INPUT_IMAGE "./images/test_image.jpg"

// Checking that parallel modes get the result as sequential
#define TEST_CONVOL(convol_mode)                                               \
  image_t *image = ctx->input_image;                                           \
  filter_t *filter = ctx->filter;                                              \
  ctx->seq_result = seq_convolution(image, filter);                            \
  ctx->parallel_result = convol_mode(image, filter);                           \
  assert_images_equal(ctx->seq_result, ctx->parallel_result);

typedef struct {
  image_t *input_image;
  filter_t *filter;
  image_t *seq_result;
  image_t *parallel_result;
} state_t;

static void assert_images_equal(image_t *expected, image_t *actual) {
  assert_non_null(expected);
  assert_non_null(actual);

  assert_int_equal(expected->width, actual->width);
  assert_int_equal(expected->height, actual->height);
  assert_int_equal(expected->channels, actual->channels);

  int size = expected->width * expected->height * expected->channels;

  for (int i = 0; i < size; i++) {
    assert_int_equal(expected->image_data[i], actual->image_data[i]);
  }
}

static int setup(void **state) {
  state_t *ctx = malloc(sizeof(state_t));
  if (!ctx) {
    return -1;
  }

  ctx->input_image = load_image(INPUT_IMAGE);
  if (!ctx->input_image) {
    free(ctx);
    return -1;
  }
  ctx->filter = create_filter(MOTION_BLUR);
  if (!ctx->filter) {
    free_image(ctx->input_image);
    free(ctx);
    return -1;
  }
  ctx->seq_result = NULL;
  ctx->parallel_result = NULL;
  *state = ctx;
  return 0;
}

static int teardown(void **state) {
  state_t *ctx = *state;
  if (ctx) {
    free_filter(ctx->filter);
    free_image(ctx->input_image);
    free_image(ctx->seq_result);
    free_image(ctx->parallel_result);
    free(ctx);
  }
  return 0;
}

static void test_pixel_convolution(void **state) {
  state_t *ctx = *state;
  TEST_CONVOL(parallel_pixel_convolution);
}

static void test_row_convolution(void **state) {
  state_t *ctx = *state;
  TEST_CONVOL(parallel_row_convolution);
}

static void test_column_convolution(void **state) {
  state_t *ctx = *state;
  TEST_CONVOL(parallel_column_convolution);
}

static void test_block_convolution(void **state) {
  state_t *ctx = *state;
  TEST_CONVOL(parallel_block_convolution);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test_setup_teardown(test_pixel_convolution, setup, teardown),
      cmocka_unit_test_setup_teardown(test_row_convolution, setup, teardown),
      cmocka_unit_test_setup_teardown(test_column_convolution, setup, teardown),
      cmocka_unit_test_setup_teardown(test_block_convolution, setup, teardown),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
