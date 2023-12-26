#ifndef PIXEL_H
#define PIXEL_H

#include <iostream>

struct Pixel {
  uint8_t r;
  uint8_t g;
  uint8_t b;

  Pixel() : r(0), g(0), b(0) {}
};

#endif