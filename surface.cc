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

void Surface::Line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
    int32_t dx = x2 - x1,
            dy = y2 - y1;
    int32_t stepx = dx > 0 ? 1 : -1,
            stepy = dy > 0 ? 1 : -1;
    int32_t x = x1 - stepx, y = y1, ny;

    while (static_cast<uint32_t>(x) != x2) {
        x += stepx;
        y -= stepy;
        ny = (dx != 0 ? y1 + (dy * (x - static_cast<int32_t>(x1))) / dx : y2);
        while (y != ny) {
            y += stepy;
            Set(x, y, 255);
       };
    };
}

}
