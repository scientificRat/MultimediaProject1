#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <string>

//This is a final class, no virtual funcs
class Image {
public:
    typedef std::uint8_t Byte;
private:
    unsigned int channels;
    unsigned int width;
    unsigned int height;

    Byte *rawData;

    void doCopy(const Image &image);

    Image();

public:

    static Image createFromBMP(const std::string &inputFileURI);

    Image(uint32_t width, uint32_t height, uint32_t channels);

    Image(const Image &image);

    Image &operator=(const Image &image);

    Image(Image &&image);

    Image &operator=(Image &&image);

    void saveToBMP(const std::string &outputFileURI);

    unsigned int getChannels() const;

    unsigned int getWidth() const;

    unsigned int getHeight() const;

    Byte *getRawData() const;

    ~Image();
};


#endif //IMAGE_H
