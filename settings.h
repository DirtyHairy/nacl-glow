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

#ifndef GLOW_SETTINGS_H
#define GLOW_SETTINGS_H

#include <stdint.h>

namespace glow {

class Settings {
    public:

        Settings();

        float Bleed() const volatile {
            return bleed;
        }
        Settings& Bleed(float bleed);

        float Decay_exp() const volatile {
            return decay_exp;
        }
        Settings& Decay_exp(float decay_exp);

        float Decay_factor() const volatile {
            return decay_factor;
        }

        uint8_t Decay_lin() const volatile {
            return decay_lin;
        }
        Settings& Decay_lin(uint8_t decay_lin);

        uint32_t Radius() const volatile {
            return radius;
        }
        Settings& Radius(uint32_t radius);

        uint8_t Fps() const volatile {
            return fps;
        }
        Settings& Fps(uint8_t fps);

    private:
        
        float bleed;
        uint8_t decay_lin, fps;
        uint32_t radius;

        float decay_exp, decay_factor;
};

}

#endif // GLOW_SETTINGS_H
