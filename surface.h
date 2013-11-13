#ifndef GLOW_GRAYSCALE_BUFFER_H
#define GLOW_GRAYSCALE_BUFFER_H

#include <stdint.h>

namespace glow {

class Surface {
    public:

        Surface(uint32_t width, uint32_t height);
        ~Surface();

        uint32_t Get(uint32_t x, uint32_t y) const {
            return buffer[y * width + x];
        }

        void Set(uint32_t x, uint32_t y, uint32_t hue) {
            buffer[y * width + x] = hue;
        }

        uint32_t GetArea() const {
            return area;
        }

        uint8_t* GetBuffer() {
            return buffer;
        }

        void Decay();
        void Line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

    private:

        uint32_t width, height, area;
        uint8_t* buffer;

        Surface(const Surface&);
        const Surface& operator=(const Surface&);
};

}

#endif // GLOW_GRAYSCALE_BUFFER_H
