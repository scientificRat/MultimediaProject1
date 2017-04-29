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

// Predefined DitherMatrix
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
 *
 * @param origin            The Origin Image
 * @param ditheringMatrix   The Dithering Matrix
 * @return Dithered image
 */
Image dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix = &DM4);

/**
 * Do ordered dithering
 *
 * @param origin                The Origin Image
 * @param ditheringMatrix       The Dithering Matrix
 * @return OrderedDithered image
 */
Image ordered_dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix = &DM4);

/**
 * Do dithering
 *
 * @param origin                The Origin Image
 * @param ditheringMatrix       Dithering Matrix in linear-array format
 * @param matrixColsOrRows      Dithering Matrix's cols or rows (side length)
 * @return
 */
Image dithering(const Image &origin, uint8_t *ditheringMatrix, uint32_t matrixColsOrRows);

/**
 *
 * @param origin             The Origin Image
 * @param ditheringMatrix    Dithering Matrix in linear-array format
 * @param colsOrRows         Dithering Matrix's cols or rows (side length)
 * @return
 */
Image ordered_dithering(const Image &origin, uint8_t *ditheringMatrix, uint32_t colsOrRows);

#endif //IMAGEPROCESS_H
