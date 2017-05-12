#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include "Image.h"
#include "ImageProcess.h"
#include "getopt.h"

#define USAGE "=== Help Info ===\n"\
              "-h : Show help info\n"\
              "-i path : Set image file path\n"\
              "-o path : Set dither type( d or o )\n"\
              "-m number(1 - 6) : Set dither matrix order(2 ^ number)\n"\
              "-t d|o : Set dither type\n"

int main(int argc, char *argv[]) {
    ::opterr = 0;
    char optch = 0;
    bool over = false;

    int matOrder = 3;
    bool dither = false;
    std::string inputPath;
    std::ostringstream outpoutPath;
    int imageCount = 0;
    std::ifstream imageTester;

    if (argc < 2) {
        std::cout << USAGE << std::endl;
        return 1;
    }

    do {
        outpoutPath.str(std::string());
        outpoutPath << "outputImage" << imageCount << ".bmp";
        ++imageCount;
        imageTester.open(outpoutPath.str());
    } while (!imageTester.fail());

    while ((optch = getopt(argc, argv, "hi:o:m:t:")) != -1 && !over) {
        switch (optch) {
            case 'h':
            case '?':
                std::cout << USAGE << std::endl;
                over = true;
                break;

            case 'i':
                inputPath = optarg;
                break;

            case 'o':
                outpoutPath.str(optarg);
                break;

            case 'm':
                try {
                    matOrder = std::stoi(optarg);
                }
                catch (std::out_of_range &err) {
                    std::cout << "Order is too large." << std::endl;
                    over = 1;
                }
                catch (std::invalid_argument &err) {
                    std::cout << "Invalid parameter " << optarg << " for matrix order." << std::endl;
                    over = 1;
                }

                if (matOrder < 1 || matOrder > 6) {
                    std::cout << "Order have to be less than 64 or larger than 1." << std::endl;
                    over = 1;
                }
                break;

            case 't':
                if ((optarg[0] != 'd' && optarg[0] != 'o') || optarg[1] != '\0') {
                    std::cout << "Invalid parameter " << optarg << " for dither type." << std::endl;
                    over = 1;
                }
                if (optarg[0] == 'd') {
                    dither = true;
                }
                break;
        }
    }

    if (!over) {
        std::cout << "Reading image from bmp file..." << std::endl;
        ImageMat image = ImageMat::createFromBMP(inputPath);
        auto mat = calcDitherMatrix(matOrder);

        if (image.getRawData() == nullptr) {
            std::cout << "Can't read bmp file " << inputPath << std::endl;
            return 1;
        }

        std::cout << "Dither processing..." << std::endl;
        if (dither) {
            image = dithering(image, mat);
        } else {
            image = ordered_dithering(image, mat);
        }

        std::cout << "Saving image file..." << std::endl;
        image.saveToBMP(outpoutPath.str());
    }

    return 0;
}