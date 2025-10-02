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

void Image::blur(int radius) {
    if (radius <= 0) return;

    int kernel_size = 2 * radius + 1;
    double sigma = radius / 2.0;  // heuristic: sigma ~ radius/2

    // --- 1. Build Gaussian kernel ---
    std::vector<double> kernel(kernel_size);
    double sum = 0.0;
    for (int i = -radius; i <= radius; i++) {
        double val = std::exp(-(i * i) / (2 * sigma * sigma));
        kernel[i + radius] = val;
        sum += val;
    }
    for (double &v : kernel) v /= sum; // normalize

    // --- 2. Temporary buffer for horizontal pass ---
    Image temp(this->width, this->height);

    // Horizontal blur
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            for (int c = 0; c < this->channels; c++) {
                double acc = 0.0;
                for (int k = -radius; k <= radius; k++) {
                    int xx = std::clamp(x + k, 0, this->width - 1);
                    acc += this->operator()(xx, y, c) * kernel[k + radius];
                }
                temp(x, y, c) = static_cast<unsigned char>(std::round(acc));
            }
        }
    }

    // --- 3. Vertical blur into output ---
    Image blurred(this->width,this->height);

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            for (int c = 0; c < this->channels; c++) {
                double acc = 0.0;
                for (int k = -radius; k <= radius; k++) {
                    int yy = std::clamp(y + k, 0, this->height - 1);
                    acc += temp(x, yy, c) * kernel[k + radius];
                }
                blurred(x, y, c) = static_cast<unsigned char>(std::round(acc));
            }
        }
    }

    // --- 4. Put back into original ---
    std::swap(this->imageData, blurred.imageData);
}

void Image::blurring(){

    int kernelSize = 21;
    int radiusKernel = kernelSize/2;

    Image image2(this->width, this->height);

    for (int i = radiusKernel; i < this->width - radiusKernel; i++)
    {
        for (int j = radiusKernel; j < this->height - radiusKernel; j++)
        {
            int rSum = 0, gSum = 0, bSum = 0;
            int nNeighbors = 0;

            for (int x = -radiusKernel; x <= radiusKernel; x++)
            {
                for (int y = -radiusKernel; y <= radiusKernel; y++)
                {
                    int xNeighbor = x + i;
                    int yNeighbor = y + j;
                    if (xNeighbor >= 0 && xNeighbor < this->width && yNeighbor >= 0 && yNeighbor < this->height)
                    {
                        rSum += this->operator()(xNeighbor, yNeighbor, 0);
                        gSum += this->operator()(xNeighbor, yNeighbor, 1);
                        bSum += this->operator()(xNeighbor, yNeighbor, 2);
                        nNeighbors++;
                    }
                }
            }
            int rAvg, gAvg, bAvg;
            if (nNeighbors > 0)
            {
                rAvg = rSum/nNeighbors;
                gAvg = gSum/nNeighbors;
                bAvg = bSum/nNeighbors;
            }
            else
            {
                rAvg = this->operator()(i, j, 0);
                gAvg = this->operator()(i, j, 1);
                bAvg = this->operator()(i, j, 2);
            }
            image2(i, j, 0) = rAvg;
            image2(i, j, 1) = gAvg;
            image2(i, j, 2) = bAvg;
        }
    }

    Image blurredImage(image2.width - radiusKernel*2, image2.height - radiusKernel*2);

    for (int i = 0; i < blurredImage.width; i++)
    {
        for (int j = 0; j < blurredImage.height; j++)
        {
            for (int c = 0; c < 3; c++)
            {
                blurredImage(i, j, c) = image2(i + radiusKernel, j + radiusKernel, c);
            }
        }
    }
    (*this) = blurredImage;
}


