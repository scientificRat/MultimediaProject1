//
// Created by 黄正跃 on 2017/4/27.
//

#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include "Image.h"

template<typename T>
struct SquareMatrix {
    T *data;
    uint32_t colsOrRows;
};

extern const SquareMatrix<uint8_t> DM4;
extern const SquareMatrix<uint8_t> DM9;
extern const SquareMatrix<uint8_t> DM16;
extern const SquareMatrix<uint8_t> DM64;


Image dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix = &DM4);

Image ordered_dithering(const Image &origin, const SquareMatrix<uint8_t> *ditheringMatrix = &DM4);

Image dithering(const Image &origin, uint8_t *ditheringMatrix, uint32_t matrixColsOrRows);

Image ordered_dithering(const Image &origin, uint8_t *ditheringMatrix, uint32_t colsOrRows);

#endif //IMAGEPROCESS_H
