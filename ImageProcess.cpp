//
// Created by 黄正跃 on 2017/4/27.
//

#include "ImageProcess.h"

/**
 * Data from <a> https://en.wikipedia.org/wiki/Ordered_dithering </a>
 */
namespace data {
    uint8_t DM4arr[] = {0, 2,
                        3, 1};

    uint8_t DM9arr[] = {0, 7, 3,
                        6, 5, 2,
                        4, 1, 8};

    uint8_t DM16arr[] = {0, 8, 2, 10,
                         12, 4, 14, 6,
                         3, 11, 1, 9,
                         15, 7, 13, 5};

    uint8_t DM64arr[] = {0, 48, 12, 60, 3, 51, 15, 63,
                         32, 16, 44, 28, 35, 19, 47, 31,
                         8, 56, 4, 52, 11, 59, 7, 55,
                         40, 24, 36, 20, 43, 27, 39, 23,
                         2, 50, 14, 62, 1, 49, 13, 61,
                         34, 18, 46, 30, 33, 17, 45, 29,
                         10, 58, 6, 54, 9, 57, 5, 53,
                         42, 26, 38, 22, 41, 25, 37, 21};

}

const SquareMatrix<uint8_t> DM4{data::DM4arr, 2};
const SquareMatrix<uint8_t> DM9{data::DM9arr, 3};
const SquareMatrix<uint8_t> DM16{data::DM16arr, 4};
const SquareMatrix<uint8_t> DM64{data::DM64arr, 8};

Image dithering(const Image &origin, uint8_t *ditheringMatrix, uint32_t matrixColsOrRows) {
    uint32_t channels = origin.getChannels();
    uint32_t originWidth = origin.getWidth();
    uint32_t originHeight = origin.getHeight();
    uint32_t rstWidth = originWidth * matrixColsOrRows;
    uint32_t rstHeight = originHeight * matrixColsOrRows;
    Image rstImage = Image(rstWidth, rstHeight, channels);
    const uint8_t *originData = origin.getRawData();
    uint8_t *rstData = rstImage.getRawData();
    uint8_t ratio = (uint8_t) (256 / (matrixColsOrRows * matrixColsOrRows));
    for (int i = 0; i < originHeight; ++i) {
        for (int j = 0; j < originWidth; ++j) {
            for (int c = 0; c < channels; ++c) {
                for (int ii = 0; ii < matrixColsOrRows; ++ii) {
                    for (int jj = 0; jj < matrixColsOrRows; ++jj) {
                        uint32_t rstPos =
                                (i * matrixColsOrRows + ii) * rstWidth * channels +
                                (j * matrixColsOrRows + jj) * channels + c;
                        uint32_t originPos = i * originWidth * channels + j * channels + c;
                        rstData[rstPos] = (uint8_t) ((originData[originPos] / ratio) >
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
    uint8_t ratio = (uint8_t) (256 / (matrixColsOrRows * matrixColsOrRows));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            uint8_t threshold = ditheringMatrix[(i % matrixColsOrRows) * matrixColsOrRows + (j % matrixColsOrRows)];
            for (int c = 0; c < channels; ++c) {
                uint32_t pos = i * width * channels + j * channels + c;
                rstData[pos] = (uint8_t) ((originData[pos] / ratio) > threshold ? 255 : 0);
            }
        }
    }
    return rstImage;
}

Image dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix) {
    return dithering(origin, ditheringMatrix->data, ditheringMatrix->colsOrRows);
}

Image ordered_dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix) {
    return ordered_dithering(origin, ditheringMatrix->data, ditheringMatrix->colsOrRows);
}
