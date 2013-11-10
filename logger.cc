#include "logger.h"

#include "ppapi/cpp/var.h"

namespace glow {

Logger::Logger(pp::Instance* instance) : instance(instance) {}

Logger::~Logger() {}

void Logger::Log(const std::string& message) {
    lock.Acquire();
    instance->LogToConsole(PP_LOGLEVEL_LOG, pp::Var(message));
    lock.Release();
}

}
