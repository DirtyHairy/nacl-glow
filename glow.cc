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

#include "instance.h"

namespace glow {

/**
 * Every NaCl program must define a module class which derives from pp::Module
 * and implements the CreateInstance factory method.
 */
class Module : public pp::Module {
    public:

        /**
         * The actual program logic goes into an Instance class which extends
         * pp::Instance. This method is required and works as a factory for new
         * Instance instances.
         */
        pp::Instance* CreateInstance(PP_Instance instance) {
            return new Instance(instance);
        }
};

}

namespace pp {

/**
 * The programs entry point is the pp::CreateModule method which creates a new
 * instance of the Module class defined by the program
 */
Module* CreateModule() {
    return new glow::Module();
}

}
