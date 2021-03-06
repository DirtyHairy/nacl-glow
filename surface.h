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

#ifndef GLOW_SURFACE_H
#define GLOW_SURFACE_H

#include <stdint.h>

namespace glow {

/**
 * The Surface class implements a 8-bit grayscale framebuffer and the provides
 * surface transformation and a couple of drawing operations. There is nothing
 * NaCl specific here, so documentation is more sparse. Sorry :)
 */
class Surface {
    public:

        Surface(uint32_t width, uint32_t height);
        ~Surface();

        uint32_t Get(uint32_t x, uint32_t y) const {
            return buffer[y * width + x];
        }

        uint32_t GetClipped(int32_t x, int32_t y) const {
            if (x >= 0 && static_cast<uint32_t>(x) < width &&
               y >= 0 && static_cast<uint32_t>(y) < height)
            {
                return buffer[y * width + x];
            } else {
                return 0;
            }
  
        }

        void Set(uint32_t x, uint32_t y, uint32_t hue) {
            buffer[y * width + x] = hue;
        }

        void SetClipped(int32_t x, int32_t y, uint32_t hue) {
            if (x >= 0 && static_cast<uint32_t>(x) < width &&
                y >= 0 && static_cast<uint32_t>(y) < height)
            {
                buffer[y * width + x] = hue;
            }
        }

        uint32_t GetArea() const {
            return area;
        }

        uint8_t* GetBuffer() {
            return buffer;
        }

        void Decay(float bleed, float decay_exp, uint8_t decay_lin);
        void Line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t r);
        void Circle(int32_t x, int32_t y, uint32_t r);

    private:

        uint32_t width, height, area;
        uint8_t* buffer, *backbuffer;

        Surface(const Surface&);
        const Surface& operator=(const Surface&);
};

}

#endif // GLOW_SURFACE_H
