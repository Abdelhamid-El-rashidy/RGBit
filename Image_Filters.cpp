#include <bits/stdc++.h>
#include  "Image_Class.h"




using namespace std;

void Image::invert() {
    for (int x = 0; x<this->width;x++) {
        for (int y = 0; y<this->height;y++) {
            for (int c = 0; c<3;c++) {
                this->operator()(x,y,c) = 255 - this->operator()(x,y,c);
            }
        }
    }
}

void Image::rotate90() {
    Image newImage(this->height,this->width);
    for (int x = 0; x<this->width;x++) {
        for (int y = 0; y<this->height;y++) {
            for (int c = 0; c<3;c++) {
                newImage((this->height-1)-y,x,c) = this->operator()(x,y,c);
            }
        }
    }
    (*this) = newImage;
}
void Image::rotate180() {
    Image newImage(this->width,this->height);
    for (int x = 0; x<this->width;x++) {
        for (int y = 0; y<this->height;y++) {
            for (int c = 0; c<3;c++) {
                newImage((this->width-1)-x,(this->height-1)-y,c) = this->operator()(x,y,c);
            }
        }
    }
    (*this) = newImage;
}
void Image::rotate270() {
    this->rotate180();
    this->rotate90();
}

void Image::frame(int size, const array<int,3> &color) {
    size = min(size, std::min(this->width, this->height));

    // Top border
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < this->width; x++) {
            for (int c = 0; c<3;c++) {
                setPixel(x, y, c, color[c]);
            }
        }
    }

    // bottom border
    for (int y = this->height - size; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            for (int c = 0; c < 3; c++) {
                setPixel(x, y, c, color[c]);
            }
        }
    }

    // Left border
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < this->height; y++) {
            for (int c = 0; c < 3; c++) {
                setPixel(x, y, c, color[c]);
            }
        }
    }

    // Right border
    for (int x = this->width - size; x < this->width; x++) {
        for (int y = 0; y < this->height; y++) {
            for (int c = 0; c < 3; c++) {
                setPixel(x, y, c, color[c]);
            }
        }
    }
}

std::vector<std::vector<double>> createGaussianKernel(int kernelSize, double sigma) {
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0.0;
    int half = kernelSize / 2;

    for (int y = -half; y <= half; y++) {
        for (int x = -half; x <= half; x++) {
            double value = std::exp(-(x*x + y*y) / (2 * sigma * sigma));
            kernel[y + half][x + half] = value;
            sum += value;
        }
    }

    // Normalize so sum = 1
    for (int i = 0; i < kernelSize; i++)
        for (int j = 0; j < kernelSize; j++)
            kernel[i][j] /= sum;

    return kernel;
}

// Gaussian blur filter
void Image::blur(int kernelSize=3, double sigma=1.0) {
    if (!imageData) return;


    auto kernel = createGaussianKernel(kernelSize, sigma);
    int half = kernelSize / 2;

    unsigned char* newImageData = (unsigned char*)malloc(width * height * channels);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double sumR = 0, sumG = 0, sumB = 0;

            for (int ky = -half; ky <= half; ky++) {
                for (int kx = -half; kx <= half; kx++) {
                    int nx = clamp(x + kx, 0, width - 1);
                    int ny = clamp(y + ky, 0, height - 1);

                    double weight = kernel[ky + half][kx + half];

                    sumR += getPixel(nx, ny, 0) * weight;
                    sumG += getPixel(nx, ny, 1) * weight;
                    sumB += getPixel(nx, ny, 2) * weight;
                }
            }

            newImageData[(y * width + x) * channels + 0] = static_cast<unsigned char>(sumR);
            newImageData[(y * width + x) * channels + 1] = static_cast<unsigned char>(sumG);
            newImageData[(y * width + x) * channels + 2] = static_cast<unsigned char>(sumB);
        }
    }

    stbi_image_free(imageData);
    imageData = newImageData;
}

QImage Image::toQImage() const {
    if (!imageData || width <= 0 || height <= 0)
        return QImage();

    QImage::Format format;
    if (channels == 3) {
        format = QImage::Format_RGB888;
    } else if (channels == 4) {
        format = QImage::Format_RGBA8888;
    } else {
        return QImage();
    }

    return QImage(imageData, width, height, width * channels, format).copy();
}


