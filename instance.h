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

#ifndef GLOW_INSTANCE_H
#define GLOW_INSTANCE_H

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/input_event.h"

#include "logger.h"
#include "renderer.h"
#include "settings.h"
#include "api.h"

namespace glow {

/**
 * The instance class derives from pp::Instance and houses the program's actual
 * logic. For each instance of the module, a separate Instance instance is
 * created by the API (by calling our factory defined on the Module class).
 */
class Instance : public pp::Instance {
    public:

        explicit Instance(PP_Instance instance);
        ~Instance();

        /**
         * The API provides virtual stub methods which we can override in order
         * to react on external events.
         */
        virtual void DidChangeView(const pp::View& view);
        virtual bool HandleInputEvent(const pp::InputEvent& event);
        virtual void HandleMessage(const pp::Var& message);

        Settings& GetSettings() {
            return settings;
        }

        Logger& GetLogger() {
            return *logger;
        }

    private:

        pp::Graphics2D* graphics;
        Logger* logger;
        Renderer* renderer;
        bool drawing;
        Settings settings;
        Api* api;
};

}

#endif // GLOW_INSTANCE_H
