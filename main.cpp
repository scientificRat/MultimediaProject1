#include <iostream>
#include "Image.h"
#include "ImageProcess.h"

using namespace std;

int main() {
    cout << "Run!" << endl;
    Image image = Image::createFromBMP("/Users/huangzhengyue/Downloads/project1_testdata/lenna.bmp");
    cout << image.getWidth() << "*" << image.getHeight() << " " << image.getChannels() << endl;
    image.saveToBMP("shit.bmp");
    dithering(image, &DM64).saveToBMP("D64.bmp");
    dithering(image, &DM4).saveToBMP("D4.bmp");
    ordered_dithering(image, &DM64).saveToBMP("OD64.bmp");
    ordered_dithering(image, &DM4).saveToBMP("OD4.bmp");
    cout << "Finished!" << endl;
    return 0;

}

