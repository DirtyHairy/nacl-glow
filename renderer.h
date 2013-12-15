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

#include <sys/time.h>

#include "ppapi/cpp/message_loop.h"
#include "ppapi/utility/threading/simple_thread.h"
#include "ppapi/utility/threading/lock.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/instance_handle.h"
#include "ppapi/cpp/point.h"


#include "logger.h"
#include "surface.h"
#include "settings.h"
#include "api.h"

namespace glow {

/**
 * The renderer handles the main loop.
 */
class Renderer {
    public:

        Renderer(
            const pp::InstanceHandle& handle,
            Logger& logger,
            Api& api,
            const volatile Settings& settings,
            pp::Graphics2D* graphics
        );
        ~Renderer();

        /*
         * Start and stop the thread
         */
        void Start();
        void Stop();

        /**
         * These turtle-like graphics methods provide the external interface
         * available to the main thread.
         */
        void MoveTo(const pp::Point& x);
        void DrawTo(const pp::Point& x);
        void SetDrawing(bool drawing);

    private:
   
        pp::InstanceHandle handle;
        Logger& logger;
        Api& api;
        pp::Graphics2D* graphics;

        /**
         * pp::SimpleThread is a simple wrapper around a pthread and also
         * automatically creates a message loop for the thread.
         */
        pp::SimpleThread* thread;

        /**
         * We need those in order to make sure that the message loop will not
         * be polled again before joining threads and stopping the renderer.
         */
        pp::Lock message_loop_lock;
        bool quit_requested;

        /**
         * pp::CompletionCallbackFactory allows to create
         * pp::CompletionCallback instances from instance methods. The
         * callbacks maintain a reference to the instance and can bind up to
         * three arbitrary arguments (copy semantics).
         */
        pp::CompletionCallbackFactory<Renderer>* callback_factory;

        Surface* surface;
        bool render_pending;
        bool drawing;

        /**
         * We are going to modify those from the main thread, so we add
         * volatile just to make sure that the compiler doesn't cache.
         */
        const volatile Settings& settings;

        pp::Point current_position;

        /**
         * The main loop.
         */
        void Dispatch();

        /**
         * The static dispatcher is called upon thread creation and transfers
         * control to the main loop on the instance.
         */
        static void DispatchThreadCallback(pp::MessageLoop&, void* userdata);

        bool PumpMessageLoop();
        void RenderSurface();
        void RenderCallback(uint32_t status);

        bool processFps(
            timeval& fps_reference,
            uint32_t& processing_counter,
            uint32_t& render_counter
        );

        void DoMoveTo(uint32_t status, const pp::Point& x);
        void DoDrawTo(uint32_t status, const pp::Point& x);
        void DoSetDrawing(uint32_t status, bool drawing);

        Renderer(const Renderer&);
        const Renderer& operator=(const Renderer&);
};

}

#endif // GLOW_RENDERER_H
