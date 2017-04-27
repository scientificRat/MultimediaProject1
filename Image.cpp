
#include "Image.h"
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
    std::uint32_t biSizeImage;
    std::int32_t biXPelsPerMeter;
    std::int32_t biYPelsPerMeter;
    std::uint32_t biClrUsed;
    std::uint32_t biClrImportant;

};
#pragma pack()


Image::Image() :
        channels(0), width(0), height(0), rawData(nullptr) {}

Image Image::createFromBMP(const std::string &inputFileURI) {
    Image image;
    BITMAP_FILE_HEADER header;
    BITMAP_INFO_HEADER info;
    std::ifstream file(inputFileURI, std::ifstream::binary);

    file.read((char *) &header, sizeof(BITMAP_FILE_HEADER))
            .read((char *) &info, sizeof(BITMAP_INFO_HEADER));

    // Get image info
    image.width = info.biWidth;
    image.height = info.biHeight > 0 ? info.biHeight : -info.biHeight;
    image.channels = info.biBitCount / 8;

    // Get image data
    int lineSkipCount =
            ((image.width * image.channels + 3) & ~0x03)
                                                 - image.width * image.channels; // Skip filled data

    image.rawData = new Image::Byte[image.width * image.height * image.channels];

    file.seekg(header.bfOffBits, file.beg);

    for (int i = 0; i < image.height; ++i) {
        char *rawDataPos = nullptr;

        if (info.biHeight > 0) {
            rawDataPos = (char *) (image.rawData +
                                   (image.height - 1 - i) * image.width * image.channels);
        } else {
            rawDataPos = (char *) (image.rawData +
                                   i * image.width * image.channels);
        }

        file.read(rawDataPos, image.width * image.channels);
        file.seekg(lineSkipCount, file.cur);
    }

    file.close();
    return image;
}

void Image::doCopy(const Image &image) {
    if (image.rawData != nullptr) {
        rawData = new Byte[image.width * image.height * image.channels];
        std::memcpy(rawData, image.rawData,
                    image.width * image.height * image.channels);
    }
}

Image::Image(const Image &image) :
        channels(image.channels), width(image.width),
        height(image.height), rawData(nullptr) {
    doCopy(image);
}

Image &Image::operator=(const Image &image) {
    delete[] rawData;

    channels = image.channels;
    width = image.width;
    height = image.height;
    rawData = nullptr;

    doCopy(image);

    return *this;
}

Image::Image(Image &&image) :
        channels(image.channels), width(image.width),
        height(image.height), rawData(image.rawData) {
    image.rawData = nullptr;
}

Image &Image::operator=(Image &&image) {
    delete[] rawData;

    channels = image.channels;
    width = image.width;
    height = image.height;
    rawData = image.rawData;

    image.rawData = nullptr;

    return *this;
}

Image::~Image() {
    delete[] rawData;
}

void Image::saveToBMP(const std::string &outputFileURI) {
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
    info.biCompression = info.biSizeImage = 0;
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

unsigned int Image::getChannels() const {
    return channels;
}

unsigned int Image::getWidth() const {
    return width;
}

unsigned int Image::getHeight() const {
    return height;
}

Image::Byte *Image::getRawData() const {
    return rawData;
}

Image::Image(uint32_t width, uint32_t height, uint32_t channels) : width(width), height(height), channels(channels) {
    rawData = new Byte[this->width * this->height * this->channels];
}
