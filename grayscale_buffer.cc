#include "grayscale_buffer.h"

namespace glow {

GrayscaleBuffer::GrayscaleBuffer(uint32_t width, uint32_t height) :
    width(width),
    height(height),
    area(width * height)
{
    buffer = new uint8_t[area];
}

GrayscaleBuffer::~GrayscaleBuffer() {
    delete[] buffer;
}

}
