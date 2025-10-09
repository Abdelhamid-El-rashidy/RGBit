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
    Image newImage(this->height,this->width);
    for (int x = 0; x<this->width;x++) {
        for (int y = 0; y<this->height;y++) {
            for (int c = 0; c<3;c++) {
                newImage(y, (this->width-1) - x, c) = this->operator()(x, y, c);
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

    const int radius = 5; const int intensityLevels = 20.0;

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


void toGray(Image& image) {
    for ( int i = 0; i < image.width ; ++i) {
        for(int j = 0 ; j < image.height ; ++j ){
            unsigned int avg = 0;
            for(int k = 0; k < 3 ; ++k) {
                avg += image(i,j,k);
            }
            avg = avg /3;
            for(int k = 0 ; k < 3 ; ++k ){
                image(i,j,k) = avg;
            }
        }
    }
}
///     Darken   and     lighteng    ///
void toDarken(Image& image) {
    for ( int i = 0; i < image.width ; ++i) {
        for(int j = 0 ; j < image.height ; ++j ){
            float factor = 0.5;
            for(int k = 0; k < 3 ; ++k) {
                image(i,j,k) = factor  *  image(i,j,k);
            }
        }
    }
}
void toLighten(Image& image) {
    for ( int i = 0; i < image.width ; ++i) {
        for(int j = 0 ; j < image.height ; ++j ){
            float factor = 1.5;
            for(int k = 0; k < 3 ; ++k) {
                if (image(i,j,k) > 127 ) {
                    image(i,j,k) = 255 ;
                }
                else {
                    image(i,j,k) = factor  *  image(i,j,k);
                }
            }
        }
    }
}
///     merge   ///
void toresize(Image& image) {
    Image resized(image.width / 3 , image.height / 3 );
    float scaleX = static_cast<float>(image.width) / resized.width;
    float scaleY = static_cast<float>(image.height) / resized.height;
    for (int i = 0; i < resized.width; i++) {
        for (int j = 0; j < resized.height; j++) {
            for (int k = 0; k < 3; k++) {
                int srcX = min((int)ceil(i * scaleX), image.width - 1);
                int srcY = min((int)ceil(j * scaleY), image.height - 1);
                resized(i, j, k) = image(srcX, srcY, k);
            }
        }
    }
    image = resized ;
    image.saveImage("resized.png");
}

void merge(Image& image1,Image& image2) {
    for ( int i = 0; i < image2.width ; ++i) {
        for(int j = 0 ; j < image2.height ; ++j ){
            float factor = 0.5;
            for(int k = 0; k < 3 ; ++k) {
                image2(i, j, k) = factor * image2(i, j, k);
            }
        }
    }
    for ( int i = 0; i < image2.width ; ++i) {
        for(int j = 0 ; j < image2.height ; ++j ){
            float factor = 0.5;
            for(int k = 0; k < 3 ; ++k) {
                image2(i, j, k) += factor * image1(i, j, k);
            }
        }
    }
}
///      detect_edges    / ////

void detect_edges(Image& image1) {
    Image change(image1.width, image1.height);
    Image image2(image1.width, image1.height);
    for (int i = 0; i < image1.width; ++i) {
        for (int j = 0; j < image1.height; ++j) {
            int gray = 0.299 * image1(i, j, 0) + 0.587 * image1(i, j, 1) + 0.114 * image1(i, j, 2);
            for (int k = 0; k < 3; ++k)
                change(i, j, k) = (gray > 127) ? 255 : 0;
        }
    }
    for (int i = 1; i < image1.width - 1; ++i) {
        for (int j = 1; j < image1.height - 1; ++j) {
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
                image2(i, j, k) = edge ? 0 : 255;
        }
    }
    image1 = image2;
}

////    sunlight   ////
void sunlight(Image& image1) {
    int centerX = image1.width / 2;
    int centerY = image1.height / 2;
    double maxDistance = sqrt(centerX * centerX + centerY * centerY);
    for (int i = 0; i < image1.width; ++i) {
        for (int j = 0; j < image1.height; ++j) {
            double dx = i - centerX;
            double dy = j - centerY;
            double distance = sqrt(dx * dx + dy * dy);
            double intensity_factor = 1.5 - (distance / maxDistance);
            if (intensity_factor < 1.0) intensity_factor = 1.0;
            for (int k = 0; k < 3; ++k) {
                int val = image1(i,j,k);
                if (k == 0) val += 60 * intensity_factor;
                if (k == 1) val += 40 * intensity_factor;
                if (k == 2) val -= 20 * intensity_factor;
                if (val > 255) val = 255;
                if (val < 0) val = 0;
                image1(i,j,k) = val;
            }
        }
    }
}


QImage Image::toQImage() const {
    if (!imageData || width <= 0 || height <= 0)
        return QImage();

    QImage::Format format;
    format = QImage::Format_RGB888;

    return QImage(imageData, width, height, width * channels, format).copy();
}

