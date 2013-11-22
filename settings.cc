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

namespace glow {

Settings::Settings() :
    bleed(0.95),
    decay_exp(0.02),
    decay_lin(1),
    fps(20),
    radius(3)
{}

Settings& Settings::Bleed(float _bleed) {
    bleed = _bleed;
    return *this;
}

Settings& Settings::Decay_exp(float _decay_exp) {
    decay_exp = _decay_exp;
    return *this;
}

Settings& Settings::Decay_lin(uint8_t _decay_lin) {
    decay_lin = _decay_lin;
    return *this;;
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
