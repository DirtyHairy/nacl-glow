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

#ifndef GLOW_API_H
#define GLOW_API_H

#include "ppapi/cpp/var.h"
#include "ppapi/utility/completion_callback_factory.h"

namespace glow {

class Instance;

/**
 * The Api class encapsulates the handles incoming and dispatches outgoing
 * messages
 */
class Api {
    public:

        Api(Instance& instance);
        ~Api();

        void HandleMessage(const pp::Var& message);

        void BroadcastFps(float processing_fps, float rendering_fps);

    private:

        Instance& instance;

        /**
         * pp::CompletionCallbackFactory allows to create
         * pp::CompletionCallback instances from instance methods. The
         * callbacks maintain a reference to the instance and can bind up to
         * three arbitrary arguments (copy semantics).
         */
        pp::CompletionCallbackFactory<Api>* callback_factory;

        void DoPostMessage(uint32_t result, const pp::Var& message);

        Api(const Api&);
        const Api& operator=(const Api&);
};

}

#endif // GLOW_API_H
