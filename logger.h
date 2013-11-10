#ifndef GLOW_LOGGER_H
#define GLOW_LOGGER_H

#include <string>

#include "ppapi/cpp/instance.h"
#include "ppapi/utility/threading/lock.h"

namespace glow {

class Logger {
    public:

        Logger(pp::Instance* instance);
        ~Logger();

        void Log(const std::string& message);

    private:

        pp::Instance* instance;
        pp::Lock lock;

        Logger(const Logger&);
        const Logger& operator=(const Logger&);
};

}

#endif // GLOW_LOGGER_H
