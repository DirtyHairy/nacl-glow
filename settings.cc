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
