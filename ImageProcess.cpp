//
// Created by 黄正跃 on 2017/4/27.
//

#include <cmath>
#include "ImageProcess.h"
#include <stdexcept>

const SquareMatrix<uint8_t> DM2({0, 2,
                                 3, 1}, 2);
const SquareMatrix<uint8_t> DM3({0, 7, 3,
                                 6, 5, 2,
                                 4, 1, 8}, 3);
const SquareMatrix<uint8_t> DM4({0, 8, 2, 10,
                                 12, 4, 14, 6,
                                 3, 11, 1, 9,
                                 15, 7, 13, 5}, 4);
const SquareMatrix<uint8_t> DM8({0, 48, 12, 60, 3, 51, 15, 63,
                                 32, 16, 44, 28, 35, 19, 47, 31,
                                 8, 56, 4, 52, 11, 59, 7, 55,
                                 40, 24, 36, 20, 43, 27, 39, 23,
                                 2, 50, 14, 62, 1, 49, 13, 61,
                                 34, 18, 46, 30, 33, 17, 45, 29,
                                 10, 58, 6, 54, 9, 57, 5, 53,
                                 42, 26, 38, 22, 41, 25, 37, 21}, 8);

ImageMat dithering(const ImageMat &origin, uint8_t *ditheringMatrix, uint32_t matrixColsOrRows) {
    uint32_t channels = origin.getChannels();
    uint32_t originWidth = origin.getWidth();
    uint32_t originHeight = origin.getHeight();
    uint32_t rstWidth = originWidth * matrixColsOrRows;
    uint32_t rstHeight = originHeight * matrixColsOrRows;
    ImageMat rstImageMat = ImageMat(rstWidth, rstHeight, channels);
    const uint8_t *originData = origin.getRawData();
    uint8_t *rstData = rstImageMat.getRawData();
    double ratio = (256.0 / (matrixColsOrRows * matrixColsOrRows + 1));
    for (int i = 0; i < originHeight; ++i) {
        for (int j = 0; j < originWidth; ++j) {
            for (int c = 0; c < channels; ++c) {
                for (int ii = 0; ii < matrixColsOrRows; ++ii) {
                    for (int jj = 0; jj < matrixColsOrRows; ++jj) {
                        uint32_t rstPos =
                                (i * matrixColsOrRows + ii) * rstWidth * channels +
                                (j * matrixColsOrRows + jj) * channels + c;
                        uint32_t originPos = i * originWidth * channels + j * channels + c;
                        rstData[rstPos] = (uint8_t) (round(originData[originPos] / ratio) >
                                                     ditheringMatrix[ii * matrixColsOrRows + jj] ? 255 : 0);
                    }
                }
            }
        }
    }
    return rstImageMat;
}

ImageMat ordered_dithering(const ImageMat &origin, uint8_t *ditheringMatrix, uint32_t matrixColsOrRows) {
    uint32_t channels = origin.getChannels();
    uint32_t width = origin.getWidth();
    uint32_t height = origin.getHeight();
    ImageMat rstImageMat = ImageMat(width, height, channels);
    const uint8_t *originData = origin.getRawData();
    uint8_t *rstData = rstImageMat.getRawData();
    double ratio = (256.0 / (matrixColsOrRows * matrixColsOrRows + 1));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            uint8_t threshold = ditheringMatrix[(i % matrixColsOrRows) * matrixColsOrRows + (j % matrixColsOrRows)];
            for (int c = 0; c < channels; ++c) {
                uint32_t pos = i * width * channels + j * channels + c;
                rstData[pos] = (uint8_t) (round(originData[pos] / ratio) > threshold ? 255 : 0);
            }
        }
    }
    return rstImageMat;
}

ImageMat dithering(const ImageMat &origin, const SquareMatrix<uint8_t> *ditheringMatrix) {
    return dithering(origin, (uint8_t *) ditheringMatrix->data.data(), ditheringMatrix->colsOrRows);
}

ImageMat ordered_dithering(const ImageMat &origin, const SquareMatrix<uint8_t> *ditheringMatrix) {
    return ordered_dithering(origin, (uint8_t *) ditheringMatrix->data.data(), ditheringMatrix->colsOrRows);
}

SquareMatrix<uint8_t> *calcDitherMatrix(uint32_t index) {
    if (index < 1) {
        throw std::runtime_error("index can't be less than 1");
    }
    if (index == 1) {
        // base case
        return new SquareMatrix<uint8_t>({0, 2, 3, 1}, 2);
    } else {
        SquareMatrix<uint8_t> *matrix = calcDitherMatrix(index - 1);
        std::vector<uint8_t> &data = matrix->data;
        // copy
        std::vector<uint8_t> tmp = data;
        uint32_t originSideLength = matrix->colsOrRows;
        uint32_t newSideLength = originSideLength * 2;
        for (int i = 0; i < tmp.size(); ++i) {
            tmp[i] *= 4;
        }
        // resize vector
        data.resize(data.size() * 4);
        for (int i = 0; i < originSideLength; ++i) {
            for (int j = 0; j < originSideLength; ++j) {
                uint8_t curr = tmp[i * originSideLength + j];
                // left-up sub-matrix
                data[i * newSideLength + j] = curr;
                // right-up sub-matrix
                data[i * newSideLength + j + originSideLength] = (uint8_t) (curr + 2);
                // left-down sub-matrix
                data[(i + originSideLength) * newSideLength + j] = (uint8_t) (curr + 3);
                // right-down sub-matrix
                data[(i + originSideLength) * newSideLength + j + originSideLength] = (uint8_t) (curr + 1);
            }
        }
        matrix->colsOrRows = newSideLength;
        return matrix;
    }
}


bool isSameSize(const ImageMat &origin, const ImageMat &output) {
    if (origin.getWidth() != output.getWidth()) {
        return false;
    }
    if (origin.getHeight() != output.getHeight()) {
        return false;
    }
    return true;
}


void cvtColorToBGR(const ImageMat &origin, ImageMat &output) {
    // Same check
    if (&origin != &output) {
        output = ImageMat(origin.getWidth(), origin.getHeight(), 3, ImageMat::BGR);
    }
    // Do convert
    uint32_t cols = origin.getHeight();
    uint32_t rows = origin.getWidth();
    uint8_t *originData = origin.getRawData();
    uint8_t *outputData = output.getRawData();
    switch (origin.getType()) {
        case ImageMat::BGR:
            // do nothing
            break;
        case ImageMat::YUV: {
            for (int i = 0; i < origin.getHeight(); ++i) {
                for (int j = 0; j < origin.getWidth(); ++j) {
                    uint8_t *originPix = originData + i * cols * 3 + j;
                    uint8_t *outputPix = outputData + i * cols * 3 + j;
                    uint8_t Y = originPix[0];
                    uint8_t U = originPix[1];
                    uint8_t V = originPix[2];
                    // R
                    outputPix[2] = (uint8_t) (1 * Y + 0.000 * U + 1.140 * V);
                    // G
                    outputPix[1] = (uint8_t) (1 * Y - 0.395 * U - 0.581 * V);
                    // B
                    outputPix[0] = (uint8_t) (1 * Y + 2.032 * U + 0.000 * V);
                }
            }
            break;
        }
        case ImageMat::YCbCr: {
            for (int i = 0; i < origin.getHeight(); ++i) {
                for (int j = 0; j < origin.getWidth(); ++j) {
                    uint8_t *originPix = originData + i * cols * 3 + j;
                    uint8_t *outputPix = outputData + i * cols * 3 + j;
                    uint8_t Y = originPix[0];
                    uint8_t Cb = originPix[1];
                    uint8_t Cr = originPix[2];
                    // R
                    outputPix[2] = (uint8_t) (1 * Y + 0.00000 * (Cb - 128) + 1.40200 * (Cr - 128));
                    // G
                    outputPix[1] = (uint8_t) (1 * Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128));
                    // B
                    outputPix[0] = (uint8_t) (1 * Y + 1.77200 * (Cb - 128) + 0.00000 * (Cr - 128));
                }
            }
            break;
        }
        case ImageMat::YIQ: {
            for (int i = 0; i < origin.getHeight(); ++i) {
                for (int j = 0; j < origin.getWidth(); ++j) {
                    uint8_t *originPix = originData + i * cols * 3 + j;
                    uint8_t *outputPix = outputData + i * cols * 3 + j;
                    uint8_t Y = originPix[0];
                    uint8_t I = originPix[1];
                    uint8_t Q = originPix[2];
                    // R
                    outputPix[2] = (uint8_t) (1 * Y + 0.956 * I + 0.623 * Q);
                    // G
                    outputPix[1] = (uint8_t) (1 * Y - 0.272 * I - 0.648 * Q);
                    // B
                    outputPix[0] = (uint8_t) (1 * Y - 1.105 * I + 0.705 * Q);
                }
            }
            break;
        }
        case ImageMat::HSI: {
            for (int i = 0; i < origin.getHeight(); ++i) {
                for (int j = 0; j < origin.getWidth(); ++j) {
                    // FIXME: some problems here
                    uint8_t *originPix = originData + i * cols * 3 + j;
                    uint8_t *outputPix = outputData + i * cols * 3 + j;
                    double H = originPix[0] / 255.0 * 360;
                    double S = originPix[1] / 255.0;
                    double I = originPix[2];
                    if (H <= 120) {
                        // B
                        outputPix[0] = (uint8_t) (I * (1 - S));
                        // R
                        outputPix[2] = (uint8_t) (I * (1 + S * cos(M_PI / 180 * H) / cos(M_PI / 180 * (60 - H))));
                        // G
                        outputPix[1] = (uint8_t) (3 * I - (outputPix[0] + outputPix[2]));
                    } else if (H <= 240) {
                        H -= 120;
                        // R
                        outputPix[2] = (uint8_t) (I * (1 - S));
                        // G
                        outputPix[1] = (uint8_t) (I * (1 + S * cos(M_PI / 180 * H) / cos(M_PI / 180 * (60 - H))));
                        // B
                        outputPix[0] = (uint8_t) (3 * I - (outputPix[2] + outputPix[1]));

                    } else {
                        H -= 240;
                        // G
                        outputPix[1] = (uint8_t) (I * (1 - S));
                        // B
                        outputPix[0] = (uint8_t) (I * (1 + S * cos(M_PI / 180 * H) / cos(M_PI / 180 * (60 - H))));
                        // R
                        outputPix[2] = (uint8_t) (3 * I - (outputPix[1] + outputPix[0]));
                    }
                }
            }
            break;
        }
        case ImageMat::Gray:
            throw std::runtime_error("Can not convert Gray to BGR");
            break;
        default:
            throw std::runtime_error("Unsupported Type");
            break;
    }
}


void cvtColor(const ImageMat &origin, ImageMat &output, ImageMat::Type outputType) {
    // Same check
    if (&origin != &output) {
        if (outputType == ImageMat::Gray) {
            output = ImageMat(origin.getWidth(), origin.getHeight(), 1, outputType);
        } else {
            output = ImageMat(origin.getWidth(), origin.getHeight(), 3, outputType);
        }
    }
    if (origin.getType() == outputType) {
        output = origin;
        return;
    }
    if (origin.getType() == ImageMat::Gray) {
        // Gray to non-gray is not supported
        throw std::runtime_error("Gray to non-gray is not supported");
    }
    // Convert to BGR
    ImageMat bgrImage(origin.getWidth(), origin.getHeight(), origin.getChannels(), ImageMat::BGR);
    cvtColorToBGR(origin, bgrImage);
    // Convert BGR To other
    uint32_t cols = origin.getHeight();
    uint32_t rows = origin.getWidth();
    uint8_t *originData = origin.getRawData();
    uint8_t *outputData = output.getRawData();
    switch (outputType) {
        case ImageMat::Gray: {
            for (int i = 0; i < origin.getHeight(); ++i) {
                for (int j = 0; j < origin.getWidth(); ++j) {
                    uint8_t *originPix = originData + i * cols * 3 + j;
                    outputData[i * cols + j] = static_cast<uint8_t >(originPix[2] * 0.299 + originPix[1] * 0.587 +
                                                                     originPix[0] * 0.114);
                }
            }
            break;
        }
        case ImageMat::YUV: {
            for (int i = 0; i < origin.getHeight(); ++i) {
                for (int j = 0; j < origin.getWidth(); ++j) {
                    uint8_t *originPix = originData + i * cols * 3 + j;
                    uint8_t *outputPix = outputData + i * cols * 3 + j;
                    uint8_t R = originPix[2];
                    uint8_t G = originPix[1];
                    uint8_t B = originPix[0];
                    // Y
                    outputPix[0] = (uint8_t) (0.299 * R + 0.587 * G + 0.114 * B);
                    // U
                    outputPix[1] = (uint8_t) (-0.148 * R - 0.289 * G + 0.437 * B);
                    // V
                    outputPix[2] = (uint8_t) (0.615 * R + 0.515 * G - 0.100 * B);
                }
            }
            break;
        }
        case ImageMat::YCbCr: {
            for (int i = 0; i < origin.getHeight(); ++i) {
                for (int j = 0; j < origin.getWidth(); ++j) {
                    uint8_t *originPix = originData + i * cols * 3 + j;
                    uint8_t *outputPix = outputData + i * cols * 3 + j;
                    uint8_t R = originPix[2];
                    uint8_t G = originPix[1];
                    uint8_t B = originPix[0];
                    // Y
                    outputPix[0] = (uint8_t) (0.290 * R + 0.587 * G + 0.114 * B);
                    // Cb
                    outputPix[1] = (uint8_t) (-0.1687 * R - 0.3313 * G + 0.500 * B + 128);
                    // Cr
                    outputPix[2] = (uint8_t) (0.5000 * R - 0.4187 * G - 0.0813 * B + 128);
                }
            }
            break;
        }
        case ImageMat::YIQ: {
            for (int i = 0; i < origin.getHeight(); ++i) {
                for (int j = 0; j < origin.getWidth(); ++j) {
                    uint8_t *originPix = originData + i * cols * 3 + j;
                    uint8_t *outputPix = outputData + i * cols * 3 + j;
                    uint8_t R = originPix[2];
                    uint8_t G = originPix[1];
                    uint8_t B = originPix[0];
                    // Y
                    outputPix[0] = (uint8_t) (0.290 * R + 0.587 * G + 0.114 * B);
                    // I
                    outputPix[1] = (uint8_t) (0.596 * R - 0.274 * G - 0.322 * B);
                    // Q
                    outputPix[2] = (uint8_t) (0.211 * R - 0.522 * G + 0.311 * B);
                }
            }
            break;
        }
        case ImageMat::HSI: {
            for (int i = 0; i < origin.getHeight(); ++i) {
                for (int j = 0; j < origin.getWidth(); ++j) {
                    uint8_t *originPix = originData + i * cols * 3 + j;
                    uint8_t *outputPix = outputData + i * cols * 3 + j;
                    uint8_t R = originPix[2];
                    uint8_t G = originPix[1];
                    uint8_t B = originPix[0];

                    double tmp = sqrt((B - G) * (B - G) + (R - B) * (G - B));
                    if (tmp == 0) {
                        throw std::runtime_error("Congratulations! H in HSI is undefined if R=G=B");
                    }
                    double theta = (180 / M_PI) * acos(0.5 * (2 * R - G - B) / tmp);
                    // H
                    if (B <= G) {
                        outputPix[0] = (uint8_t) (theta * 255.0 / 360);
                    } else {
                        outputPix[0] = (uint8_t) ((360 - theta) * 255.0 / 360);
                    }
                    // S
                    uint8_t minSubpixel = R;
                    if (G < minSubpixel) {
                        minSubpixel = G;
                    }
                    if (B < minSubpixel) {
                        minSubpixel = B;
                    }
                    int sum = R + G + B;
                    if (sum == 0) {
                        throw std::runtime_error("Congratulations! S in HSI is undefined if R+G+B=0");
                    }
                    outputPix[1] = (uint8_t) ((1 - 3.0 * minSubpixel / sum) * 255);
                    // I
                    outputPix[2] = (uint8_t) (sum / 3);
                }
            }
            break;
        }
        case ImageMat::BGR:
            break;
        default:
            throw std::runtime_error("Unsupported Type");
            break;

    }

}