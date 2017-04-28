//
// Created by 黄正跃 on 2017/4/27.
//

#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <vector>
#include "Image.h"

template<typename T>
struct SquareMatrix {

    std::vector<T> data;
    uint32_t colsOrRows;

    SquareMatrix() {
    }

    SquareMatrix(std::initializer_list<uint8_t> list, uint32_t n) : data(list), colsOrRows(n) {
    }

};

extern const SquareMatrix<uint8_t> DM2;
extern const SquareMatrix<uint8_t> DM3;
extern const SquareMatrix<uint8_t> DM4;
extern const SquareMatrix<uint8_t> DM8;

/**
 * Calculate the dither matrix which side length equals 2^index
 *
 * @param index     SideLength = 2^index
 * @return          The pointer to dither matrix, <b> Don't forget to delete! </b>
 */
SquareMatrix<uint8_t> *calcDitherMatrix(uint32_t index);

/**
 * Do dithering
 * @param origin  Origin Image
 * @param ditheringMatrix  dithering Matrix
 * @return Dithered image
 */
Image dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix = &DM4);

/**
 * Do orderd dithering
 * @param origin  Origin Image
 * @param ditheringMatrix  dithering Matrix
 * @return OrderedDithered image
 */
Image ordered_dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix = &DM4);

Image dithering(const Image &origin, uint8_t *ditheringMatrix, uint32_t matrixColsOrRows);

Image ordered_dithering(const Image &origin, uint8_t *ditheringMatrix, uint32_t colsOrRows);

#endif //IMAGEPROCESS_H
