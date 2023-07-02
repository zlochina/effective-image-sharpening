# Effective Image Sharpening

This script is designed to sharpen an image using a specific convolution mask and calculate the histogram of the sharpened image. The sharpening operation is performed by convolving the image with the following mask:

![image_sharpening.jpeg](https://s3.eu-west-2.amazonaws.com/zlochinus.aws.bucket/public+access/github_pics/sharpening_matrix.jpeg)

## Requirements

- C compiler
- Portable Pixmap Format (PPM) image file as input

## Usage

1. Compile the script using a C compiler:

   ```
   gcc main.c -o image_sharpening
   ```

2. Run the script with the input image file as the argument:

   ```
   ./image_sharpening input.ppm
   ```

   Make sure to replace `input.ppm` with the path to your input image file.

3. The sharpened image will be saved as `output.ppm`, and the histogram will be saved as `output.txt`.

## Input Image Format

The input image should be in the binary encoded Portable Pixmap Format (PPM). The file should start with the following header:

```
P6
<width> <height>
255
```

Where `<width>` and `<height>` are the dimensions of the image. After the header, the file should contain the RGB values of each pixel.

## Grayscale Calculation

The script converts the RGB image to grayscale before performing the sharpening operation. The grayscale value of each pixel is calculated using the formula:

```
grayscale = 0.2126 * red + 0.7152 * green + 0.0722 * blue
```

Where `red`, `green`, and `blue` are the RGB values of the pixel. This formula represents the luminosity method for converting an RGB image to grayscale, taking into account the perceived brightness of each color channel.

## Output

The script will produce two files:

- `output.ppm`: This file will contain the sharpened image in the PPM format.
- `output.txt`: This file will contain the histogram of the sharpened image. The histogram will be divided into five intervals:

  - Interval 1: 0 to 50
  - Interval 2: 51 to 101
  - Interval 3: 102 to 152
  - Interval 4: 153 to 203
  - Interval 5: 204 to 255

  The histogram values will be written in the order: `<interval1_count> <interval2_count> <interval3_count> <interval4_count> <interval5_count>`.

## Optimization

The script aims to optimize memory usage and cache efficiency. It utilizes buffer allocation and processes the image row by row, saving and convoluting only the necessary pixels. This optimization helps in efficient memory utilization and cache management.

## Author

Zlochevskyi Vladyslav (zlochina). This script was written as a part of exercises for subject APO of the CTU FEE.

P.S. Do not copypaste if you're a student of CTU FEE, instead this script should serve you as an inspiration or a guidebook in how you acquaire optimised script.

