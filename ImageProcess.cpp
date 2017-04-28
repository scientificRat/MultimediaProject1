//
// Created by 黄正跃 on 2017/4/27.
//

#include <cmath>
#include "ImageProcess.h"

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

Image dithering(const Image &origin, uint8_t *ditheringMatrix, uint32_t matrixColsOrRows) {
    uint32_t channels = origin.getChannels();
    uint32_t originWidth = origin.getWidth();
    uint32_t originHeight = origin.getHeight();
    uint32_t rstWidth = originWidth * matrixColsOrRows;
    uint32_t rstHeight = originHeight * matrixColsOrRows;
    Image rstImage = Image(rstWidth, rstHeight, channels);
    const uint8_t *originData = origin.getRawData();
    uint8_t *rstData = rstImage.getRawData();
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
    return rstImage;
}

Image ordered_dithering(const Image &origin, uint8_t *ditheringMatrix, uint32_t matrixColsOrRows) {
    uint32_t channels = origin.getChannels();
    uint32_t width = origin.getWidth();
    uint32_t height = origin.getHeight();
    Image rstImage = Image(width, height, channels);
    const uint8_t *originData = origin.getRawData();
    uint8_t *rstData = rstImage.getRawData();
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
    return rstImage;
}

Image dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix) {
    return dithering(origin, (uint8_t *) ditheringMatrix->data.data(), ditheringMatrix->colsOrRows);
}

Image ordered_dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix) {
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
