//
// Created by 黄正跃 on 2017/4/27.
//

#ifndef ImageMatPROCESS_H
#define ImageMatPROCESS_H

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
 * @param origin            The Origin ImageMat
 * @param ditheringMatrix   The Dithering Matrix
 * @return Dithered ImageMat
 */
ImageMat dithering(const ImageMat &origin, const SquareMatrix<uint8_t> *ditheringMatrix = &DM4);

/**
 * Do ordered dithering
 *
 * @param origin                The Origin ImageMat
 * @param ditheringMatrix       The Dithering Matrix
 * @return OrderedDithered ImageMat
 */
ImageMat ordered_dithering(const ImageMat &origin, const SquareMatrix<uint8_t> *ditheringMatrix = &DM4);

/**
 * Do dithering
 *
 * @param origin                The Origin ImageMat
 * @param ditheringMatrix       Dithering Matrix in linear-array format
 * @param matrixColsOrRows      Dithering Matrix's cols or rows (side length)
 * @return
 */
ImageMat dithering(const ImageMat &origin, uint8_t *ditheringMatrix, uint32_t matrixColsOrRows);

/**
 *
 * @param origin             The Origin ImageMat
 * @param ditheringMatrix    Dithering Matrix in linear-array format
 * @param colsOrRows         Dithering Matrix's cols or rows (side length)
 * @return
 */
ImageMat ordered_dithering(const ImageMat &origin, uint8_t *ditheringMatrix, uint32_t colsOrRows);

/**
 * Convert color (origin and output can be the same)
 *
 * @param origin
 * @param output
 * @param outputType
 */
void cvtColor(const ImageMat &origin, ImageMat &output, ImageMat::Type outputType);

#endif //ImageMatPROCESS_H
