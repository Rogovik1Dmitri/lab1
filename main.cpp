#include <iostream>
#include "bmp.h"
#include <fstream>
#include <cmath>
#include <vector>

int computeBitmapRowSize(BMPHeader header) {
    int bytesPerPixel = header.bitsPerPixel / 8;
    int rowSizeBytes = (header.width * bytesPerPixel + 3) & ~3;
    return rowSizeBytes;
}

BMP loadBitmap(std::string filename) {
    BMP image;

    std::ifstream inputFile(filename, std::ios::binary);
   if (!inputFile.is_open()) {
        throw std::runtime_error("Unable to open the BMP file for reading.");
    }

    inputFile.read((char*)(&image.header), sizeof(BMPHeader));
    int bytesToSkip = image.header.dataOffset - sizeof(BMPHeader);

    inputFile.seekg(bytesToSkip, std::ios::cur);

    int rowSize = computeBitmapRowSize(image.header);
    int paddedSize = rowSize * image.header.height;

    image.pixels = new unsigned char[paddedSize];
    inputFile.read((char*)(image.pixels), paddedSize);

    inputFile.close();
    return image;
}

void saveBitmap(std::string filename, BMP image) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Unable to create the BMP file for writing.");
    }

    int rowSize = computeBitmapRowSize(image.header);

    outputFile.write((char*)(&image.header), sizeof(BMPHeader));

    int bytesToWrite = image.header.dataOffset - sizeof(BMPHeader);
    for (int i = 0; i < bytesToWrite; i++) {
        char extraByte = 0;
        outputFile.write(&extraByte, 1);
    }

    for (int y = 0; y < image.header.height; y++) {
        int pixelOffset = y * rowSize;
        outputFile.write((char*)(image.pixels + pixelOffset), rowSize);
    }

    outputFile.close();
}

BMP rotateBitmapClockwise(BMP image) {
    BMP newImage;

    int bpp = image.header.bitsPerPixel / 8;

    newImage.header = image.header;
    newImage.header.width += newImage.header.height;
    newImage.header.height = newImage.header.width - newImage.header.height;
    newImage.header.width -= newImage.header.height;

    int newRowSize = ((newImage.header.width * bpp + 3) / 4) * 4;
    int oldRowSize = computeBitmapRowSize(image.header);

    newImage.header.fileSize = sizeof(BMPHeader) + newRowSize * newImage.header.height;
    newImage.header.dataSize = newRowSize * newImage.header.height;

    newImage.pixels = new unsigned char[newImage.header.dataSize];

    for (int x = 0; x < newImage.header.height; x++) {
        for (int y = 0; y < newImage.header.width; y++) {
            int oldOffset = y * oldRowSize + x * bpp;
            int newOffset = (newImage.header.height - 1 - x) * newRowSize + y * bpp;

            for (int channel = 0; channel < bpp; channel++) {
                newImage.pixels[newOffset + channel] = image.pixels[oldOffset + channel];
            }
        }
    }

    return newImage;
}

BMP rotateBitmapCounterclockwise(BMP image) {
    BMP newImage;

    int bpp = image.header.bitsPerPixel / 8;

    newImage.header = image.header;
    newImage.header.width += newImage.header.height;
    newImage.header.height = newImage.header.width - newImage.header.height;
    newImage.header.width -= newImage.header.height;

    int newRowSize = ((newImage.header.width * bpp + 3) / 4) * 4;
    int oldRowSize = computeBitmapRowSize(image.header);

    newImage.header.fileSize = sizeof(BMPHeader) + newRowSize * newImage.header.height;
    newImage.header.dataSize = newRowSize * newImage.header.height;

    newImage.pixels = new unsigned char[newImage.header.dataSize];

    for (int x = 0; x < newImage.header.height; x++) {
        for (int y = 0; y < newImage.header.width; y++) {
            int oldOffset = y * oldRowSize + x * bpp;
            int newOffset = x * newRowSize + (newImage.header.width - 1 - y) * bpp;

            for (int channel = 0; channel < bpp; channel++) {
                newImage.pixels[newOffset + channel] = image.pixels[oldOffset + channel];
            }
        }
    }

    return newImage;
}

void applyGaussianSmoothing(BMP& image, double sigma) {
    int bpp = image.header.bitsPerPixel / 8;

    int rowSize = ((image.header.width * bpp + 3) / 4) * 4;
    int kernelSize = (6 * sigma) + 1;
    double *kernel = new double[kernelSize];

    for (int i = 0; i < kernelSize; i++) {
        double x = i - (kernelSize - 1) / 2;
        double value = std::exp(-(x * x) / (2 * sigma * sigma));
        kernel[i] = value;
    }

    double sum = 0.0;
    for (int i = 0; i < kernelSize; i++) {
        sum += (kernel[i]);
    }
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] /= sum;
    }

    BMP smoothedImage = image;
    for (int y = 0; y < image.header.height; y++) {
        for (int x = 0; x < image.header.width; x++) {
            std::vector<double> colorChannels(bpp, 0.0);

            for (int i = 0; i < kernelSize; i++) {
                int posX = x + i - (kernelSize - 1) / 2;
                if (posX < 0) posX = 0;
                if (posX >= image.header.width) posX = image.header.width - 1;

                int pixelOffset = y * rowSize + posX * bpp;

                for (int channel = 0; channel < bpp; channel++) {
                    colorChannels[channel] +=
                        (double)(image.pixels[pixelOffset + channel]) * kernel[i];
                }
            }

            int pixelOffset = y * rowSize + x * bpp;
            for (int channel = 0; channel < bpp; channel++) {
                smoothedImage.pixels[pixelOffset + channel] =
                    (unsigned char)(colorChannels[channel]);
            }
        }
    }

    delete[] kernel;
    kernel = nullptr;
    image = smoothedImage;
}

int main() {
    try {
        BMP image = loadBitmap("1234.bmp");

        BMP rotatedImageRight = rotateBitmapClockwise(image);
        saveBitmap("rotated_right_123.bmp", rotatedImageRight);
        delete[] rotatedImageRight.pixels;
        rotatedImageRight.pixels = nullptr;

        //BMP rotatedImageLeft = rotateBitmapCounterclock
        BMP rotatedImageLeft = rotateBitmapCounterclockwise(image);
        saveBitmap("rotated_left_123.bmp", rotatedImageLeft);
        delete[] rotatedImageLeft.pixels;
        rotatedImageLeft.pixels = nullptr;

        double sigma = 7.0; 
        applyGaussianSmoothing(image, sigma);
        saveBitmap("blurred_123.bmp", image);
        delete[] image.pixels;
        image.pixels = nullptr;

    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
