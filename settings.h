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

/**
 * The Settings object is a container for the various parameters controlling
 * the rendering process. We will be calling the getters from the rendering
 * thread, while the main thread will call the setters, so the getters are
 * marked as volatile in order to ensure that no values are cached in
 * registers. As any race between getters and setters is not harmful to the
 * program logic, we don't need to use mutexes to ensure exclusive access.
 */
class Settings {
    public:

        Settings();

        float Bleed() const volatile {
            return bleed;
        }
        Settings& Bleed(float bleed);

        /**
         * The decay factor is not well suited for direct slider control,
         * so we map it to 15 minus the amount of half-time frames. The decay
         * factor is calculated from this on the fly.
         */
        float Decay_exp() const volatile {
            return decay_exp;
        }
        Settings& Decay_exp(float decay_exp);

        /**
         * See above.
         */
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
