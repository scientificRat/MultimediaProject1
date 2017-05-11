#include "ImageMat.h"
#include <fstream>

#pragma pack(1)
struct BITMAP_FILE_HEADER {
    std::uint16_t bfType;
    std::uint32_t bfSize;
    std::uint16_t bfReserved1;
    std::uint16_t bfReserved2;
    std::uint32_t bfOffBits;
};

#pragma pack(1)
struct BITMAP_INFO_HEADER {
    std::uint32_t biSize;
    std::int32_t biWidth;
    std::int32_t biHeight;
    std::uint16_t biPlanes;
    std::uint16_t biBitCount;
    std::uint32_t biCompression;
    std::uint32_t biSizeImageMat;
    std::int32_t biXPelsPerMeter;
    std::int32_t biYPelsPerMeter;
    std::uint32_t biClrUsed;
    std::uint32_t biClrImportant;
};
#pragma pack()



ImageMat::ImageMat() :
        channels(0), width(0), height(0), rawData(nullptr), Type(RGB)
		{}

ImageMat ImageMat::createFromBMP(const std::string &inputFileURI) {
    ImageMat ImageMat;
    BITMAP_FILE_HEADER header;
    BITMAP_INFO_HEADER info;
    std::ifstream file(inputFileURI, std::ifstream::binary);

    file.read((char *) &header, sizeof(BITMAP_FILE_HEADER))
            .read((char *) &info, sizeof(BITMAP_INFO_HEADER));

    // Get ImageMat info
    ImageMat.width = info.biWidth;
    ImageMat.height = info.biHeight > 0 ? info.biHeight : -info.biHeight;
    ImageMat.channels = info.biBitCount / 8;

    // Get ImageMat data
    int lineSkipCount =
            ((ImageMat.width * ImageMat.channels + 3) & ~0x03)
                                                 - ImageMat.width * ImageMat.channels; // Skip filled data

    ImageMat.rawData = new ImageMat::Byte[ImageMat.width * ImageMat.height * ImageMat.channels];

    file.seekg(header.bfOffBits, file.beg);

    for (int i = 0; i < ImageMat.height; ++i) {
        char *rawDataPos = nullptr;

        if (info.biHeight > 0) {
            rawDataPos = (char *) (ImageMat.rawData +
                                   (ImageMat.height - 1 - i) * ImageMat.width * ImageMat.channels);
        } else {
            rawDataPos = (char *) (ImageMat.rawData +
                                   i * ImageMat.width * ImageMat.channels);
        }

        file.read(rawDataPos, ImageMat.width * ImageMat.channels);
        file.seekg(lineSkipCount, file.cur);
    }

    file.close();
    return ImageMat;
}

void ImageMat::doCopy(const ImageMat &ImageMat) {
    if (ImageMat.rawData != nullptr) {
        rawData = new Byte[ImageMat.width * ImageMat.height * ImageMat.channels];
        std::memcpy(rawData, ImageMat.rawData,
                    ImageMat.width * ImageMat.height * ImageMat.channels);
    }
}

ImageMat::ImageMat(const ImageMat &ImageMat) :
        channels(ImageMat.channels), width(ImageMat.width),
        height(ImageMat.height), rawData(nullptr) {
    doCopy(ImageMat);
}

ImageMat &ImageMat::operator=(const ImageMat &ImageMat) {
    delete[] rawData;

    channels = ImageMat.channels;
    width = ImageMat.width;
    height = ImageMat.height;
    rawData = nullptr;

    doCopy(ImageMat);

    return *this;
}

ImageMat::ImageMat(ImageMat &&ImageMat) :
        channels(ImageMat.channels), width(ImageMat.width),
        height(ImageMat.height), rawData(ImageMat.rawData) {
    ImageMat.rawData = nullptr;
}

ImageMat &ImageMat::operator=(ImageMat &&ImageMat) {
    delete[] rawData;

    channels = ImageMat.channels;
    width = ImageMat.width;
    height = ImageMat.height;
    rawData = ImageMat.rawData;

    ImageMat.rawData = nullptr;

    return *this;
}

ImageMat::~ImageMat() {
    delete[] rawData;
}

void ImageMat::saveToBMP(const std::string &outputFileURI) {
    BITMAP_FILE_HEADER header;
    BITMAP_INFO_HEADER info;
    std::ofstream file(outputFileURI, std::ofstream::binary);

    // Fill header
    int lineSize = (width * channels + 3) & ~0x03;
    header.bfType = 0x4d42;
    header.bfSize = channels == 1 ?
                    1078 + lineSize * height :
                    54 + lineSize * height;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits = channels == 1 ? 1078 : 54;
    file.write((const char *) &header, 14);

    // Fill info
    info.biSize = 40;
    info.biWidth = width;
    info.biHeight = height;
    info.biPlanes = 1;
    info.biBitCount = 8 * channels;
    info.biCompression = info.biSizeImageMat = 0;
    info.biXPelsPerMeter = info.biYPelsPerMeter = 0;
    info.biClrUsed = info.biClrImportant = channels == 1 ? 256 : 0;
    file.write((const char *) &info, 40);

    // Fill data
    if (channels == 1) {
        char rgbColor[4] = {0};
        for (int i = 0; i < 256; ++i) {
            rgbColor[0] = rgbColor[1] = rgbColor[2] = i;
            file.write(rgbColor, 4);
        }
    }

    const char *rawDataPos = nullptr;
    char emptyZone[4] = {0};
    for (int i = height - 1; i >= 0; --i) {
        rawDataPos = (const char *) rawData + i * width * channels;
        file.write(rawDataPos, width * channels);
        file.write(emptyZone, lineSize - width * channels);
    }
    file.close();
}

ImageMat::ImageMat(uint32_t width, uint32_t height, uint32_t channels, Type type) : 
	width(width), height(height), channels(channels), type(type) {
    rawData = new Byte[this->width * this->height * this->channels];
}
