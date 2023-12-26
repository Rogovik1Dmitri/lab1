#include <algorithm>
#include <fstream>
#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

double PI = 3.141592653589793238462643383279;

struct BMP {
    int width;
    int height;
    unsigned char header[54];
    unsigned char* pixels;
    int size;
    int row_padded;
    long long int size_padded;
};

void writeBMP(string filename, BMP image) {
    string fileName = filename;
    FILE* out = fopen(fileName.c_str(), "wb");
    fwrite(image.header, sizeof(unsigned char), 54, out);

    unsigned char tmp;
    for (int i = 0; i < image.height; i++) {
        for (int j = 0; j < image.width * 3; j += 3) {
            // Convert (B, G, R) to (R, G, B)
            tmp = image.pixels[j];
            image.pixels[j] = image.pixels[j + 2];
            image.pixels[j + 2] = tmp;
        }
        fwrite(&image.pixels[i * image.row_padded], sizeof(unsigned char), image.width * 3, out);
    }
    
    fclose(out);
}

BMP readBMP(string filename) {
    BMP image;
    string fileName = filename;
    FILE* in = fopen(fileName.c_str(), "rb");

    if (in == NULL)
        throw "Argument Exception";

    fread(image.header, sizeof(unsigned char), 54, in); // read the 54-byte header

     // extract image height and width from header
    image.width = *(int*)&image.header[18];
    image.height = *(int*)&image.header[22];

    image.row_padded = (image.width * 3 + 3) & (~3);     // size of a single row rounded up to multiple of 4
    image.size_padded = image.row_padded * image.height;  // padded full size
    image.pixels = new unsigned char[image.size_padded];  // allocate memory for pixels

    if (fread(image.pixels, sizeof(unsigned char), image.size_padded, in) != image.size_padded) {
        cout << "Error: all bytes couldn't be read" << endl;
    }

    fclose(in);
    return image;
}

BMP rotate90Degree(BMP image) {
    

    BMP newImage = image;
    unsigned char *pixels = new unsigned char[image.size_padded];

    int H = image.height, W = image.width;
    for (int x = 0; x < H; x++) {
        for (int y = 0; y < W;y ++) {
            pixels[(x * H + y) * 3 + 0] = image.pixels[((W - 1 - y )*  H + x )* 3 + 0];
            pixels[(x * H + y) * 3 + 1] = image.pixels[((W - 1 - y )*  H + x )* 3 + 1];
            pixels[(x * H + y) * 3 + 2] = image.pixels[((W - 1 - y )*  H + x )* 3 + 2];
        }
    }

    newImage.pixels = pixels;
    return newImage;
}
BMP rotate90DegreeR(BMP image) {
    

    BMP newImage = image;
    unsigned char *pixels = new unsigned char[image.size_padded];

    int H = image.height, W = image.width;
    for (int x = 0; x < H; x++) {
        for (int y = 0; y < W;y ++) {
            pixels[(x * H + y) * 3 + 0] = image.pixels[(y * H  +  H - 1 - x )* 3 + 0];
            pixels[(x * H + y) * 3 + 1] = image.pixels[(y * H  +  H - 1 - x )* 3 + 1];
            pixels[(x * H + y) * 3 + 2] = image.pixels[(y * H  +  H - 1 - x )* 3 + 2];
        }
    }

    newImage.pixels = pixels;
    return newImage;
}



BMP applyBlurEffect(BMP image) {
    BMP newImage = image;
    
    int H = image.height, W = image.width;
    
    for (int x = 1; x < H - 1; x++) {
        for (int y = 1; y < W - 1; y++) {
            unsigned char r = 0, g = 0, b = 0;
            
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    r += image.pixels[((x + i) * W + (y + j)) * 3 + 0];
                    g += image.pixels[((x + i) * W + (y + j)) * 3 + 1];
                    b += image.pixels[((x + i) * W + (y + j)) * 3 + 2];
                }
            }
            
            newImage.pixels[(x * W + y) * 3 + 0] = r / 9;
            newImage.pixels[(x * W + y) * 3 + 1] = g / 9;
            newImage.pixels[(x * W + y) * 3 + 2] = b /9;
        }
    }

    return newImage;
}

int main() {
    BMP image = readBMP("123.bmp");
    
    // Rotate the image to the right
    BMP rotatedLeftImage = rotate90Degree(image);
    writeBMP("rotated_left_123.bmp", rotatedLeftImage);

    BMP rotatedRightImage = rotate90DegreeR(image);
    writeBMP("rotated_Right_123.bmp", rotatedRightImage);
    
    // Apply blur effect
    BMP blurredImage = applyBlurEffect(image);
    writeBMP("blurred_123.bmp", blurredImage);
    
    return 0;
}