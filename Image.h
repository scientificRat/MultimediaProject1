#ifndef ImageMat_H
#define ImageMat_H

#include <cstdint>
#include <string>

//This is a final class, no virtual funcs
class ImageMat {
public:
    typedef std::uint8_t Byte;
    enum Type {
        BGR, YUV, YIQ, YCbCr, HSI, Gray
    };

private:
    uint32_t channels;
    uint32_t width;
    uint32_t height;
    Type type;

    Byte *rawData;

    void doCopy(const ImageMat &ImageMat);

public:

    static ImageMat createFromBMP(const std::string &inputFileURI);

    ImageMat();

    ImageMat(uint32_t width, uint32_t height, uint32_t channels, Type type = BGR);

    ImageMat(const ImageMat &imageMat);

    ImageMat &operator=(const ImageMat &imageMat);

    ImageMat(ImageMat &&imageMat) :
            channels(imageMat.channels), width(imageMat.width),
            height(imageMat.height), rawData(imageMat.rawData) {
        imageMat.rawData = nullptr;
    }

    ImageMat &operator=(ImageMat &&imageMat) {
        delete[] rawData;
        channels = imageMat.channels;
        width = imageMat.width;
        height = imageMat.height;
        rawData = imageMat.rawData;
        imageMat.rawData = nullptr;
        return *this;
    }

    void saveToBMP(const std::string &outputFileURI);

    uint32_t getChannels() const { return channels; }

    uint32_t getWidth() const { return width; }

    uint32_t getHeight() const { return height; }

    Type getType() const { return type; }

    void setType(Type type) {
        ImageMat::type = type;
    }

    Byte *getRawData() const { return rawData; }

    ~ImageMat();
};


#endif //ImageMat_H
