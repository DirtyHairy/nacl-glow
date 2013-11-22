#ifndef GLOW_SETTINGS_H
#define GLOW_SETTINGS_H

#include <stdint.h>

namespace glow {

class Settings {
    public:

        Settings();

        float Bleed() const {
            return bleed;
        }
        Settings& Bleed(float bleed);

        float Decay_exp() const {
            return decay_exp;
        }
        Settings& Decay_exp(float decay_exp);

        uint8_t Decay_lin() const {
            return decay_lin;
        }
        Settings& Decay_lin(uint8_t decay_lin);

        uint32_t Radius() const {
            return radius;
        }
        Settings& Radius(uint32_t radius);

        uint8_t Fps() const {
            return fps;
        }
        Settings& Fps(uint8_t fps);

    private:
        
        float bleed, decay_exp;
        uint8_t decay_lin, fps;
        uint32_t radius;
};

}

#endif // GLOW_SETTINGS_H
