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

#include "settings.h"

#include <cmath>

namespace {

/**
 * The setters enforce parameter constraints where applicable, so we define
 * a helper.
 */
template<typename T> T constrain(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

}

namespace glow {

Settings::Settings() :
    bleed(0.8),
    decay_lin(1),
    fps(20),
    radius(5)
{
    Decay_exp(10.);
}

Settings& Settings::Bleed(float _bleed) {
    bleed = constrain(_bleed, 0.f, 1.f);
    return *this;
}

Settings& Settings::Decay_exp(float _decay_exp) {
    decay_exp = constrain(_decay_exp, 0.f, 15.f);
    decay_factor = decay_exp == 0 ? 0 : powf(0.5, (15. - decay_exp));
    return *this;
}

Settings& Settings::Decay_lin(uint8_t _decay_lin) {
    decay_lin = _decay_lin;
    return *this;
}

Settings& Settings::Radius(uint32_t _radius) {
    radius = _radius;
    return *this;
}

Settings& Settings::Fps(uint8_t _fps) {
    fps = _fps;
    return *this;
}

}
