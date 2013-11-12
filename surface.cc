#include "surface.h"

#include <cstring>

namespace glow {

Surface::Surface(uint32_t width, uint32_t height) :
    width(width),
    height(height),
    area(width * height)
{
    buffer = new uint8_t[area];
    memset(buffer, 0, area);
}

Surface::~Surface() {
    delete[] buffer;
}

void Surface::Decay() {
    int32_t hue;

    for (uint32_t i = 0; i < area; i++) {
        hue = (buffer[i] * 95) / 100 - 1;
        buffer[i] = hue > 0 ? hue : 0;
    }
}

}
