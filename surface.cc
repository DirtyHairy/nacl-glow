/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013 Christian Speckner <cnspeckn@googlemail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "surface.h"

#include <cstring>
#include <cmath>

namespace glow {

Surface::Surface(uint32_t width, uint32_t height) :
    width(width),
    height(height),
    area(width * height)
{
    buffer = new uint8_t[area];
    backbuffer = new uint8_t[area];
    memset(buffer, 0, area);
}

Surface::~Surface() {
    delete[] buffer;
    delete[] backbuffer;
}

void Surface::Decay(float bleed, float decay_exp, uint8_t decay_lin) {
    // We use integer arithmetics in order to steer clear of potential
    // performance hits on ARM. In order to increase numeric accuracy, the
    // 8-bit grayscale values are mapped to 32 bits by multiplication /
    // division. This factor is chosen to maximize precision while avoiding
    // overflows.
    const uint32_t base = 1 << 20;

    int32_t     bleed_neightbours = nearbyint(bleed / 8. * static_cast<float>(base)),
                bleed_center = nearbyint((1. - bleed) * static_cast<float>(base)),
                decay_factor = nearbyint((1. - decay_exp) * static_cast<float>(base));

    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0; y < height; y++) {
            int32_t hue = 0;
            
            if (bleed_neightbours > 0) {
                    hue += bleed_neightbours * (
                        GetClipped(x-1, y-1) + GetClipped(x, y-1) + GetClipped(x+1, y-1) +
                        GetClipped(x-1, y) + GetClipped(x+1, y) +
                        GetClipped(x-1, y+1) + GetClipped(x, y+1) + GetClipped(x+1, y+1)
                    );
                hue += bleed_center * Get(x, y);
                hue /= base;
            } else {
                hue = Get(x, y);
            }
            hue *= decay_factor;
            hue /= base;
            hue -= decay_lin;

            uint32_t offset = y * width + x;
            if (hue < 0) {
                backbuffer[offset] = 0;
            } else if (hue > 255) {
                backbuffer[offset] = 255;
            } else {
                backbuffer[offset] = hue;
            }
        }
    }

    uint8_t* tmp = buffer;
    buffer = backbuffer;
    backbuffer = tmp;
}

void Surface::Circle(int32_t x, int32_t y, uint32_t r) {
    uint32_t r2 = r * r;
    uint32_t dx, dx2, dy;
    for (dx = 0; dx <= r; dx++) {
        dx2 = dx * dx;
        for (dy = 0; dx2 + dy*dy <= r2; dy++) {
            SetClipped(x + dx, y + dy, 255);
            SetClipped(x + dx, y - dy, 255);
            SetClipped(x - dx, y + dy, 255);
            SetClipped(x - dx, y - dy, 255);
        }
    }
}

void Surface::Line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t r) {
    if (x1 >= width || x2 >= width || y1 >= height || y2 >= height) return;

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
            Circle(x, y, r);
       };
    };
}

}
