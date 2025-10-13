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
                newImage(y, this->width - 1 - x, c) = this->operator()(x, y, c);
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
                newImage(this->width - 1 - x, this->height - 1 - y, c) = this->operator()(x, y, c);
            }
        }
    }
    (*this) = newImage;
}
void Image::rotate270() {
    Image newImage(this->height,this->width);
    for (int x = 0; x<this->width;x++) {
        for (int y = 0; y<this->height;y++) {
            for (int c = 0; c<3;c++) {
                newImage(this->height - 1 - y, x, c) = this->operator()(x, y, c);
            }
        }
    }
    (*this) = newImage;
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

vector<vector<double>> create_kernel(int kernelSize, double sigma) {
    vector<vector<double>> kernel(kernelSize, vector<double>(kernelSize));
    double sum = 0.0;
    int half = kernelSize / 2;

    for (int x = -half; x <= half; x++) {
        for (int y = -half; y <= half; y++) {
            double value = exp(-(x*x + y*y) / (2 * sigma * sigma));
            kernel[x + half][y + half] = value;
            sum += value;
        }
    }

    // Normalize to sum = 1, to ensure brightness is the same
    for (int i = 0; i < kernelSize; i++)
        for (int j = 0; j < kernelSize; j++)
            kernel[i][j] /= sum;

    return kernel;
}

void Image::blur(int size, double sigma) {
    if (!imageData) return;

    int kernelSize = (2 * size) + 1;
    kernelSize = max(kernelSize, 11);
    auto kernel = create_kernel(kernelSize, sigma);
    int half = kernelSize / 2;

    Image newImage(this->width,this->height);
    int numThreads = thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;

    auto go = [&](int startY, int endY) {
        for (int y = startY; y < endY; y++) {
            for (int x = 0; x < width; x++) {
                double sumR = 0, sumG = 0, sumB = 0;

                for (int kx = -half; kx <= half; kx++) {
                    for (int ky = -half; ky <= half; ky++) {
                        int nx = clamp(x + kx, 0, width - 1);
                        int ny = clamp(y + ky, 0, height - 1);
                        double weight = kernel[kx + half][ky + half];

                        sumR += getPixel(nx, ny, 0) * weight;
                        sumG += getPixel(nx, ny, 1) * weight;
                        sumB += getPixel(nx, ny, 2) * weight;
                    }
                }

                newImage(x,y,0) = static_cast<unsigned char>(sumR);
                newImage(x,y,1) = static_cast<unsigned char>(sumG);
                newImage(x,y,2) = static_cast<unsigned char>(sumB);
            }
        }
    };

    vector<thread> threads;
    int rowsPerThread = height / numThreads;
    int start = 0;

    for (int i = 0; i < numThreads; i++) {
        int end = (i == numThreads - 1) ? height : start + rowsPerThread;
        threads.emplace_back(go, start, end);
        start = end;
    }

    for (auto& th : threads) th.join();

    (*this) = newImage;
}

void Image::oilPainting() {
    if (!imageData) return;

    const int radius = 5; const int intensityLevels = 128.0;

    Image newImage(width, height);

    for (int y = radius; y < height - radius; ++y) {
        for (int x = radius; x < width - radius; ++x) {

            vector<int> intensityCount(intensityLevels, 0);
            vector<int> sumR(intensityLevels, 0);
            vector<int> sumG(intensityLevels, 0);
            vector<int> sumB(intensityLevels, 0);

            for (int ny = -radius; ny <= radius; ++ny) {
                for (int nx = -radius; nx <= radius; ++nx) {
                    int px = x + nx;
                    int py = y + ny;

                    int r = getPixel(px, py, 0);
                    int g = getPixel(px, py, 1);
                    int b = getPixel(px, py, 2);

                    // Standard formulea to calculate the inntensity
                    double intensity = (0.299 * r + 0.587 * g + 0.114 * b) * intensityLevels / 255.0;
                    int index = clamp((int)intensity, 0, intensityLevels - 1);

                    intensityCount[index]++;
                    sumR[index] += r;
                    sumG[index] += g;
                    sumB[index] += b;
                }
            }

            int mx = 0;
            int dominant = 0;
            for (int i = 0; i < intensityLevels; i++) {
                if (intensityCount[i] > mx) {
                    mx = intensityCount[i];
                    dominant = i;
                }
            }
            if (mx > 0) {
                newImage(x,y,0) = sumR[dominant] / mx;
                newImage(x,y,1) = sumG[dominant] / mx;
                newImage(x,y,2) = sumB[dominant] / mx;
            }
        }
    }

    (*this) = newImage;
}

void Image::skew(double angleDeg, int dir) {
    long double angle = angleDeg*M_PI/180.0;
    long double scale = tan(angle);

    int newWidth = width + fabs(height/scale);
    Image newimage(newWidth, height);

    for (int y = 0; y < height; y++) {
        int row = (dir == 0) ? (height-1)-y : y;
        int offset = (int)round(row * scale);
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                int nx = x + offset;
                if (nx >= 0 && nx < newWidth)
                    newimage(nx, y, c) = getPixel(x, y, c);
            }
        }
    }

    *this = newimage;
}

void Image::toGray() {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            unsigned int avg = 0;
            for (int k = 0; k < 3; ++k) {
                avg += (*this)(i, j, k);
            }
            avg = avg / 3;
            for (int k = 0; k < 3; ++k) {
                (*this)(i, j, k) = avg;
            }
        }
    }
}

void Image::toDarken() {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            float factor = 0.5;
            for (int k = 0; k < 3; ++k) {
                (*this)(i, j, k) = factor * (*this)(i, j, k);
            }
        }
    }
}

void Image::toLighten() {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            float factor = 1.5;
            for (int k = 0; k < 3; ++k) {
                if ((*this)(i, j, k) > 127) {
                    (*this)(i, j, k) = 255;
                } else {
                    (*this)(i, j, k) = factor * (*this)(i, j, k);
                }
            }
        }
    }
}

Image Image::merge(Image& img2) {
    int minwidth = min(width, img2.width);
    int minheight = min(height, img2.height);

    Image resized1(minwidth, minheight);
    Image resized2(minwidth, minheight);

    float scaleX1 = static_cast<float>(width) / minwidth;
    float scaleY1 = static_cast<float>(height) / minheight;
    float scaleX2 = static_cast<float>(img2.width) / minwidth;
    float scaleY2 = static_cast<float>(img2.height) / minheight;

    for (int i = 0; i < minwidth; i++) {
        for (int j = 0; j < minheight; j++) {
            int x1 = min((int)floor(i * scaleX1), width - 1);
            int y1 = min((int)floor(j * scaleY1), height - 1);
            int x2 = min((int)floor(i * scaleX2), img2.width - 1);
            int y2 = min((int)floor(j * scaleY2), img2.height - 1);

            for (int k = 0; k < 3; k++) {
                int val1 = (*this)(x1, y1, k);
                int val2 = img2(x2, y2, k);

                int blended = static_cast<int>(0.5 * val1 + 0.5 * val2);
                resized1(i, j, k) = clamp(blended, 0, 255);
            }
        }
    }

    return resized1;
}
void Image::detect_edges() {
    Image change(width, height);
    Image result(width, height);
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            int gray = 0.299 * (*this)(i, j, 0) + 0.587 * (*this)(i, j, 1) + 0.114 * (*this)(i, j, 2);
            for (int k = 0; k < 3; ++k)
                change(i, j, k) = (gray > 127) ? 255 : 0;
        }
    }
    for (int i = 1; i < width - 1; ++i) {
        for (int j = 1; j < height - 1; ++j) {
            bool edge = false;
            if (change(i, j, 0) == 0) {
                if (
                    change(i+1,j,0)==255 || change(i-1,j,0)==255 ||
                    change(i,j+1,0)==255 || change(i,j-1,0)==255 ||
                    change(i+1,j+1,0)==255 || change(i+1,j-1,0)==255 ||
                    change(i-1,j+1,0)==255 || change(i-1,j-1,0)==255
                ) {
                    edge = true;
                }
            }
            for (int k = 0; k < 3; ++k)
                result(i, j, k) = edge ? 0 : 255;
        }
    }
    *this = result;
}

void Image::sunlight() {
    int centerX = width / 2;
    int centerY = height / 2;
    double maxDistance = sqrt(centerX * centerX + centerY * centerY);
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            double dx = i - centerX;
            double dy = j - centerY;
            double distance = sqrt(dx * dx + dy * dy);
            double intensity_factor = 1.5 - (distance / maxDistance);
            if (intensity_factor < 1.0) intensity_factor = 1.0;
            for (int k = 0; k < 3; ++k) {
                int val = (*this)(i,j,k);
                if (k == 0) val += 60 * intensity_factor;
                if (k == 1) val += 40 * intensity_factor;
                if (k == 2) val -= 20 * intensity_factor;
                if (val > 255) val = 255;
                if (val < 0) val = 0;
                (*this)(i,j,k) = val;
            }
        }
    }
}

void Image::resize(int newWidth, int newHeight) {
    Image resized(newWidth, newHeight);
    float scaleX = static_cast<float>(width) / newWidth;
    float scaleY = static_cast<float>(height) / newHeight;
    for (int i = 0; i < newWidth; i++) {
        for (int j = 0; j < newHeight; j++) {
            for (int k = 0; k < 3; k++) {
                int srcX = min((int)ceil(i * scaleX), width - 1);
                int srcY = min((int)ceil(j * scaleY), height - 1);
                resized(i, j, k) = (*this)(srcX, srcY, k);
            }
        }
    }
    *this = resized;
}

void Image::purple() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            (*this)(i,j,0) = std::min(255, (*this)(i,j,0) + 10);
            (*this)(i,j,1) = std::max(0, (*this)(i,j,1) - 15);
            (*this)(i,j,2) = std::min(255, (*this)(i,j,2) + 20);
        }
    }
}

void Image::crop(int x, int y, int nwidth, int nheight) {
    Image crop(nwidth, nheight);
    if (x + nwidth > width || y + nheight > height) {
        throw("Invalid Crop");
        return;
    }
    for (int i = 0; i < nwidth; i++) {
        for (int j = 0; j < nheight; j++) {
            for (int k = 0; k < 3; k++) {
                crop(i, j, k) = (*this)(i + x, j + y, k);
            }
        }
    }
    *this = crop;
}

void Image::Black_White() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int sum = 0;
            for (int k = 0; k < 3; k++) {
                sum += (*this)(i, j, k);
            }
            int val = ((sum / 3) >= 100 ? 255 : 0);
            for (int k = 0; k < 3; k++) {
                (*this)(i, j, k) = val;
            }
        }
    }
}

void Image::flipH() {
    for (int i = 0; i < width / 2; i++) {
        for (int j = 0; j < height; j++) {
            for (int k = 0; k < channels; k++) {
                int tmp = (*this)(i, j, k);
                (*this)(i, j, k) = (*this)(width - i - 1, j, k);
                (*this)(width - i - 1, j, k) = tmp;
            }
        }
    }
}

void Image::flipV() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height / 2; j++) {
            for (int k = 0; k < channels; k++) {
                int tmp = (*this)(i, j, k);
                (*this)(i, j, k) = (*this)(i, height - j - 1, k);
                (*this)(i, height - j - 1, k) = tmp;
            }
        }
    }
}

void Image::dark_light(int op) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            for (int k = 0; k < 3; ++k) {
                if (op)
                    (*this)(i, j, k) = std::min(255, int((*this)(i, j, k) * 1.5));
                else
                    (*this)(i, j, k) = floor(0.5 * (*this)(i, j, k));
            }
        }
    }
}

void Image::infrared() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int gray = (*this)(i, j, 0);
            gray = min(255, max(0, gray));
            int adjustedRed = static_cast<int>(gray * 1.8);
            int finalRed = min(255, max(0, adjustedRed));


            (*this)(i, j, 0) = finalRed;
            (*this)(i, j, 1) = 0;        
            (*this)(i, j, 2) = 0;
        }
    }
}

void Image::emboss() {
    int kernel[3][3] = {
        {-2,-1,0}, {-1,1,1}, {0,1,2}
    };
    const int kSize = 3; int half = kSize / 2;
    Image newImage(width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pixelValue[3] = {0, 0, 0};

            for (int dy = -half; dy <= half; dy++) {
                for (int dx = -half; dx <= half; dx++) {
                    int ny = clamp(y + dy, 0, height - 1);
                    int nx = clamp(x + dx, 0, width - 1);
                    for (int c = 0; c < 3; c++) {
                        pixelValue[c] += (*this)(nx, ny, c) * kernel[dy+half][dx+half];
                    }
                }
            }

            for (int c = 0; c < 3; c++) {
                pixelValue[c] = clamp(pixelValue[c], 0, 255);
                newImage(x, y, c) = static_cast<unsigned char>(pixelValue[c]);
            }
        }
    }

    *this = newImage;

}

QImage Image::toQImage() const {
    if (!imageData || width <= 0 || height <= 0)
        return QImage();

    QImage::Format format;
    format = QImage::Format_RGB888;

    return QImage(imageData, width, height, width * channels, format).copy();
}

