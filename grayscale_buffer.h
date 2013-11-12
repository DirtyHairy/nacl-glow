#ifndef GLOW_GRAYSCALE_BUFFER_H
#define GLOW_GRAYSCALE_BUFFER_H

#include <stdint.h>

namespace glow {

class GrayscaleBuffer {
    public:

        GrayscaleBuffer(uint32_t width, uint32_t height);
        ~GrayscaleBuffer();

        uint32_t Get(uint32_t x, uint32_t y) const {
            return buffer[x * width + y];
        }

        void Set(uint32_t x, uint32_t y, uint32_t hue) {
            buffer[x * width + y] = hue;
        }

        uint32_t GetArea() const {
            return area;
        }

        uint8_t* GetBuffer() {
            return buffer;
        }

    private:

        uint32_t width, height, area;
        uint8_t* buffer;

        GrayscaleBuffer(const GrayscaleBuffer&);
        const GrayscaleBuffer& operator=(const GrayscaleBuffer&);
};

}

#endif // GLOW_GRAYSCALE_BUFFER_H
