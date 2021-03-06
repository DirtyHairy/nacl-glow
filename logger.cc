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

#include "logger.h"

#include "ppapi/cpp/var.h"

namespace glow {

Logger::Logger(pp::Instance& instance) : instance(instance) {}

Logger::~Logger() {}

/**
 * pp::Instance::LogToConsole logs a message on the javascript console. As the
 * docu doesn't say anything about thread safety, we use a mutex to be sure no
 * races can occur.
 */
void Logger::Log(const std::string& message) {
    lock.Acquire();

    // Don't be fooled by the fact that we directly pass a string, the compiled
    // code converts this to a pp::Var by calling the proper constructor.
    instance.LogToConsole(PP_LOGLEVEL_LOG, message);

    lock.Release();
}

}
