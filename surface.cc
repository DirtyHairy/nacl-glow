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
            Set(x, y, 255);
       };
    };
}

}
