#include <iostream>
#include "Image.h"
#include "ImageProcess.h"

using namespace std;

int main() {
    cout << "Run!" << endl;
    Image image = Image::createFromBMP("/Users/huangzhengyue/Desktop/barbara_1.bmp");
    cout << image.getWidth() << "*" << image.getHeight() << " " << image.getChannels() << endl;
    image.saveToBMP("origin.bmp");

    SquareMatrix<uint8_t> *dm2 = calcDitherMatrix(1);
    SquareMatrix<uint8_t> *dm4 = calcDitherMatrix(2);
    SquareMatrix<uint8_t> *dm8 = calcDitherMatrix(3);

    dithering(image, dm2).saveToBMP("D2.bmp");
    dithering(image, dm4).saveToBMP("D4.bmp");
    dithering(image, dm8).saveToBMP("D8.bmp");
    ordered_dithering(image, dm2).saveToBMP("OD2.bmp");
    ordered_dithering(image, dm4).saveToBMP("OD4.bmp");
    ordered_dithering(image, dm8).saveToBMP("OD8.bmp");
    // Display dm8
    for (int i = 0; i < dm8->data.size(); ++i) {
        if (i % dm8->colsOrRows == 0) {
            printf("\n");
        }
        printf("%02d ", dm8->data[i]);
    }

    delete dm2;
    delete dm4;
    delete dm8;
    cout << "\n\nFinished!\n";
    return 0;

}

