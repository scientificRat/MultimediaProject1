#ifndef ImageMat_H
#define ImageMat_H

#include <cstdint>
#include <string>

//This is a final class, no virtual funcs
class ImageMat {
public:
    typedef std::uint8_t Byte;
    enum Type {
    	RGB, YUV, YIQ, YCbCr, HSI
	};
    
private:
    uint32_t channels;
    uint32_t width;
    uint32_t height;
    Type type;

    Byte *rawData;

    void doCopy(const ImageMat &ImageMat);

    ImageMat();
    ImageMat(uint32_t width, uint32_t height, uint32_t channels, Type type);

public:
    static ImageMat createFromBMP(const std::string &inputFileURI);

    ImageMat(const ImageMat &ImageMat);

    ImageMat &operator=(const ImageMat &ImageMat);

    ImageMat(ImageMat &&ImageMat);

    ImageMat &operator=(ImageMat &&ImageMat);

    void saveToBMP(const std::string &outputFileURI);

    uint32_t getChannels() const { return channels; }

    uint32_t getWidth() const { return width; }

    uint32_t getHeight() const { return height; }
    
    Type getType() const { return type; }

    Byte *getRawData() const;

    ~ImageMat();
};


#endif //ImageMat_H
