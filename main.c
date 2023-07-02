/*
 * @author: Zlochevskyi Vladyslav (zlochina or zlochvla)
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define OUTPUT_FILENAME "output.ppm"
#define OUTPUT_TXT_FILENAME "output.txt"
#define CHAR_ZERO '0'
#define TRUE 1
#define FALSE 0
#define HISTOGRAM_SIZE 5

#define L1_BLOCK_SIZE 64
#define L1_SIZE 32768
#define L2_BLOCK_SIZE 64
#define L2_SIZE 1048576

// functions
void skip_row(FILE *);
int get_int_from_file(FILE *);
void read_headers(FILE *, int *, int *);

// DEBUG function
void print(int x) { printf("%d\n", (uint8_t)x); }

int main(int argc, char **argv) {
  char *filename = argc > 1 ? argv[1] : NULL;

  if (!filename) {
    fprintf(stderr, "ERROR: no filename was given\n");
    return 101;
  }

  // open file
  FILE *pic = fopen(filename, "rb");
  FILE *pic_out = fopen(OUTPUT_FILENAME, "wb");

  if (!pic) {
    fprintf(stderr, "ERROR: bad name was given\n");
    return 102;
  }

  // read headers
  int width, height;
  read_headers(pic, &width, &height);

  // write headers to output file
  fprintf(pic_out, "P6\n%d\n%d\n255\n", width, height);

  uint32_t histogram[HISTOGRAM_SIZE] = {0};
  uint8_t *pic_buff, *out_buff;
  uint16_t buffer_size = width * 9;
  uint16_t actual_width = width * 3;
  uint16_t width_array_value = actual_width - 3;

  // buffer allocation
  pic_buff = (uint8_t *)malloc(buffer_size * sizeof(uint8_t));
  if (!pic_buff) {
    fprintf(stderr, "ERROR: invalid allocation\n");
    return 103;
  }
  out_buff = (uint8_t *)malloc(actual_width * sizeof(uint8_t));
  if (!out_buff) {
    fprintf(stderr, "ERROR: invalid allocation\n");
    return 103;
  }

  uint8_t hist_val;
  int16_t red_val, green_val, blue_val;

  // process_image
  // idea: manually save first two rows,
  // then come up with solution to convolute ignoring first and last pixel
  // paralelly histogram should be calculated
  //
  // first line save to buffer and to pic_out
  uint16_t offset_x = 0;
  fread(pic_buff, sizeof(uint8_t), actual_width, pic);
  fwrite(pic_buff, sizeof(uint8_t), actual_width, pic_out);

  for (uint16_t x = 0; x < width; x++) {
    hist_val =
        round(0.2126 * pic_buff[offset_x] + 0.7152 * pic_buff[offset_x + 1] +
              0.0722 * pic_buff[offset_x + 2]);

    if (hist_val < 51) {
      histogram[0]++;
    } else if (hist_val < 102) {
      histogram[1]++;
    } else if (hist_val < 153) {
      histogram[2]++;
    } else if (hist_val < 204) {
      histogram[3]++;
    } else {
      histogram[4]++;
    }

    offset_x += 3;
  }

  // second line save to buffer only
  fread(pic_buff + actual_width, sizeof(uint8_t), actual_width, pic);
  offset_x <<= 1;  // += actual_width

  uint16_t offset_pre = actual_width;
  uint16_t offset_pre_d = 0;
  // int new_pixel;

  // third line and so on
  for (uint16_t y = 2; y < height; y++) {
    offset_x %= buffer_size;
    offset_pre_d %= buffer_size;
    offset_pre %= buffer_size;

    fread(pic_buff + offset_x, sizeof(uint8_t), actual_width, pic);
    // first pixel in row
    red_val = pic_buff[offset_pre];
    *out_buff = red_val;

    green_val = pic_buff[offset_pre + 1];
    *(out_buff + 1) = green_val;

    blue_val = pic_buff[offset_pre + 2];
    *(out_buff + 2) = blue_val;

    hist_val = round(0.2126 * red_val + 0.7152 * green_val + 0.0722 * blue_val);

    if (hist_val < 51) {
      histogram[0]++;
    } else if (hist_val < 102) {
      histogram[1]++;
    } else if (hist_val < 153) {
      histogram[2]++;
    } else if (hist_val < 204) {
      histogram[3]++;
    } else {
      histogram[4]++;
    }
    offset_x += 3;
    offset_pre_d += 3;
    offset_pre += 3;
    for (uint16_t x = 3; x < width_array_value; x += 3) {
      // Red channel
      // current + pre-previous line
      red_val = -pic_buff[offset_x] - pic_buff[offset_pre_d];
      // previous line
      red_val += (-pic_buff[offset_pre - 3] + 5 * pic_buff[offset_pre] -
                  pic_buff[offset_pre + 3]);

      if (red_val < 0) red_val = 0;
      if (red_val > 0xff) red_val = 0xff;
      *(out_buff + x) = red_val;

      // Green channel
      // current + pre-previous line
      green_val = -pic_buff[offset_x + 1] - pic_buff[offset_pre_d + 1];
      // previous line
      green_val += (-pic_buff[offset_pre - 2] + 5 * pic_buff[offset_pre + 1] -
                    pic_buff[offset_pre + 4]);

      if (green_val < 0) green_val = 0;
      if (green_val > 0xff) green_val = 0xff;

      *(out_buff + x + 1) = green_val;

      // Blue channel
      // current + pre-previous line
      blue_val = -pic_buff[offset_x + 2] - pic_buff[offset_pre_d + 2];
      // previous line
      blue_val += (-pic_buff[offset_pre - 1] + 5 * pic_buff[offset_pre + 2] -
                   pic_buff[offset_pre + 5]);

      if (blue_val < 0) blue_val = 0;
      if (blue_val > 0xff) blue_val = 0xff;

      *(out_buff + x + 2) = blue_val;

      offset_x += 3;
      offset_pre_d += 3;
      offset_pre += 3;

      hist_val =
          round(0.2126 * red_val + 0.7152 * green_val + 0.0722 * blue_val);

      if (hist_val < 51) {
        histogram[0]++;
      } else if (hist_val < 102) {
        histogram[1]++;
      } else if (hist_val < 153) {
        histogram[2]++;
      } else if (hist_val < 204) {
        histogram[3]++;
      } else {
        histogram[4]++;
      }
    }

    // Last pixel process
    red_val = pic_buff[offset_pre];
    *(out_buff + actual_width - 3) = red_val;

    green_val = pic_buff[offset_pre + 1];
    *(out_buff + actual_width - 2) = green_val;

    blue_val = pic_buff[offset_pre + 2];
    *(out_buff + actual_width - 1) = blue_val;

    hist_val = round(0.2126 * red_val + 0.7152 * green_val + 0.0722 * blue_val);

    if (hist_val < 51) {
      histogram[0]++;
    } else if (hist_val < 102) {
      histogram[1]++;
    } else if (hist_val < 153) {
      histogram[2]++;
    } else if (hist_val < 204) {
      histogram[3]++;
    } else {
      histogram[4]++;
    }

    offset_x += 3;
    offset_pre_d += 3;
    offset_pre += 3;
    fwrite(out_buff, sizeof(uint8_t), actual_width, pic_out);
  }

  // last line load from buffer to output file
  //
  offset_x = (offset_x + 2 * actual_width) % buffer_size;
  fwrite(pic_buff + offset_x, sizeof(uint8_t), actual_width, pic_out);

  for (uint16_t x = 0; x < width; x++) {
    hist_val =
        round(0.2126 * pic_buff[offset_x] + 0.7152 * pic_buff[offset_x + 1] +
              0.0722 * pic_buff[offset_x + 2]);

    offset_x += 3;
    if (hist_val < 51) {
      histogram[0]++;
    } else if (hist_val < 102) {
      histogram[1]++;
    } else if (hist_val < 153) {
      histogram[2]++;
    } else if (hist_val < 204) {
      histogram[3]++;
    } else {
      histogram[4]++;
    }
  }

  // save histogram values to output file
  FILE *histogram_file = fopen(OUTPUT_TXT_FILENAME, "w");

  fprintf(histogram_file, "%d %d %d %d %d", histogram[0], histogram[1],
          histogram[2], histogram[3], histogram[4]);

  // close files
  if (pic) {
    fclose(pic);
    fclose(pic_out);
    fclose(histogram_file);
    free(pic_buff);
    free(out_buff);
  }

  return EXIT_SUCCESS;
}

void read_headers(FILE *file, int *width, int *height) {
  skip_row(file);  // skip "P6"
  *width = get_int_from_file(file), *height = get_int_from_file(file);
  skip_row(file);  // skip pixel max value "255"
}

int get_int_from_file(FILE *file) {
  char char_tmp = fgetc(file);
  int integer = char_tmp - CHAR_ZERO;

  while (TRUE) {
    char char_tmp = fgetc(file);

    if (char_tmp == '\n') {
      break;
    }
    integer = integer * 10 + char_tmp - CHAR_ZERO;
  }
  return integer;
}

void skip_row(FILE *file) {
  char char_tmp = fgetc(file);
  while (char_tmp != '\n') {
    char_tmp = fgetc(file);
  }
}
