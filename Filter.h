#ifndef H_Filter
#define H_Filter

#include <vector>

#include "pixle.h"

class Filter {
 public:
  virtual ~Filter() = default;
  virtual void apply(Pixel** image, int height, int width) = 0;
};

#endif