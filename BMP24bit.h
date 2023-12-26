#ifndef H_BMP24bit
#define H_BMP24bit

#include <vector>

#include "Filter.h"
#include "pixle.h"

class BMP24bit {
 private:
#pragma pack(push, 1)
  struct BMPHeader {
    unsigned short int type;
    int size;
    unsigned short int reserved1;
    unsigned short int reserved2;
    int bits_offset;
    int header_in_file_size;
    int width;
    int height;
    unsigned short int bcPlanes;
    unsigned short int bits_per_pixel;
  };
#pragma pack(pop)

  BMPHeader* header;

  Pixel** pixels;
  unsigned char* bytes_between_header_and_pixels;

 public:
  BMP24bit(const std::string& filename);
  ~BMP24bit();
  void rotate_left();
  void rotate_right();
  void apply_filter(Filter* filter);
  void save(const std::string& filename);
};

#endif