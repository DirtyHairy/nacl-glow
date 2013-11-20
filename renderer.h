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

#ifndef GLOW_RENDERER_H
#define GLOW_RENDERER_H

#include "ppapi/utility/threading/simple_thread.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/instance_handle.h"
#include "ppapi/cpp/point.h"

#include "logger.h"
#include "surface.h"

namespace glow {

class Renderer {
    public:

        Renderer(
            const pp::InstanceHandle& handle,
            Logger* logger,
            pp::Graphics2D* graphics
        );
        ~Renderer();

        void Start();
        void Stop();

        void MoveTo(const pp::Point& x);
        void DrawTo(const pp::Point& x);
        void SetDrawing(bool drawing);

        void _Dispatch();

    private:
   
        pp::InstanceHandle handle;
        Logger* logger;
        pp::Graphics2D* graphics;
        pp::SimpleThread* thread;
        pp::CompletionCallbackFactory<Renderer>* callback_factory;
        Surface* surface;
        bool render_pending;

        pp::Point current_position;
        bool drawing;
        uint32_t radius;

        void RenderSurface();
        void RenderCallback(uint32_t status);

        void DoMoveTo(uint32_t status, const pp::Point& x);
        void DoDrawTo(uint32_t status, const pp::Point& x);
        void DoSetDrawing(uint32_t status, bool drawing);

        Renderer(const Renderer&);
        const Renderer& operator=(const Renderer&);
};

}

#endif // GLOW_RENDERER_H
