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

#include "renderer.h"

#include "unistd.h"

#include "ppapi/cpp/message_loop.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/point.h"
#include "ppapi/cpp/size.h"
#include "ppapi/cpp/message_loop.h"

namespace {

void dispatch(pp::MessageLoop&, void* user_data) {
    static_cast<glow::Renderer*>(user_data)->_Dispatch();
}


int32_t TimeDifference(const timeval& t1, const timeval& t2) {
    return (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
}

uint32_t delay(const timeval& reference, uint32_t delay)
{
    timeval current;
    if (gettimeofday(&current, NULL) != 0) return 0;

    int32_t actual_delay = delay - TimeDifference(reference, current);

    if (actual_delay > static_cast<int32_t>(delay)) {
        actual_delay = delay;
    }

    if (actual_delay > 0) {
        usleep(actual_delay);
    }
    return actual_delay;
}

inline uint32_t PixelRGB(const uint8_t r, const uint8_t g, const uint8_t b) {
    return 0xFF000000 | (b << 16) | (g << 8) | r;
}

class EQuit {};

}

namespace glow {

Renderer::Renderer(
    const pp::InstanceHandle& handle,
    Logger& logger,
    Api& api,
    const Settings& settings,
    pp::Graphics2D* graphics)
:
   handle(handle),
   logger(logger),
   api(api),
   graphics(graphics),
   thread(NULL),
   surface(NULL),
   drawing(false),
   settings(settings)
{
    callback_factory = new pp::CompletionCallbackFactory<Renderer>(this);
}

Renderer::~Renderer() {
    Stop();

    delete callback_factory;
}

void Renderer::Start() {
    if (thread != NULL) {
        return;
    }

    thread = new pp::SimpleThread(handle);
    thread->StartWithFunction(&dispatch, this);
}

void Renderer::Stop() {
    if (thread == NULL) {
        return;
    }

    thread->Join();
    delete thread;
    thread = NULL;

    delete surface;
}

void Renderer::MoveTo(const pp::Point& x) {
    thread->message_loop().PostWork(callback_factory->NewCallback(
        &Renderer::DoMoveTo, x)
    );
}

void Renderer::DrawTo(const pp::Point& x) {
    thread->message_loop().PostWork(callback_factory->NewCallback(
        &Renderer::DoDrawTo, x)
    );
}

void Renderer::SetDrawing(bool isDrawing) {
    thread->message_loop().PostWork(callback_factory->NewCallback(
        &Renderer::DoSetDrawing, isDrawing)
    );
}

void Renderer::DoMoveTo(uint32_t status, const pp::Point& x) {
    current_position = x;
}

void Renderer::DoDrawTo(uint32_t status, const pp::Point& x) {
    if (surface != NULL) surface->Line(
        current_position.x(), current_position.y(),
        x.x(), x.y(), settings.Radius()
    );
    current_position = x;
}

void Renderer::DoSetDrawing(uint32_t status, bool isDrawing) {
    drawing = isDrawing;
}

void Renderer::_Dispatch() {
    pp::Size extent = graphics->size();
    surface = new Surface(extent.width(), extent.height());

    timeval timestamp, fps_reference;
    pp::MessageLoop& message_loop = thread->message_loop();
    uint32_t render_counter = 0, processing_counter = 0;

    try {
        if (gettimeofday(&fps_reference, NULL) != 0) throw EQuit();

        while (true) {
            if (gettimeofday(&timestamp, NULL) != 0) throw EQuit();

            surface->Decay(
                settings.Bleed(),
                settings.Decay_exp(),
                settings.Decay_lin()
            );

            if (message_loop.PostQuit(false) != PP_OK) throw EQuit();
            if (message_loop.Run() != PP_OK) throw EQuit();

            if (drawing){
                surface->Circle(
                    current_position.x(),
                    current_position.y(),
                    settings.Radius()
                );
            }

            if (!render_pending) {
                render_counter++;
                render_pending = true;
                RenderSurface();
            }

            processing_counter++;

            processFps(fps_reference, processing_counter, render_counter);

            delay(timestamp, 1000000 / settings.Fps());
        }
    }
    catch(EQuit) {}
}

void Renderer::processFps(
    timeval& fps_reference,
    uint32_t& processing_counter,
    uint32_t& rendering_counter)
{
    timeval measurement;
    if (gettimeofday(&measurement, NULL) != 0) throw EQuit();

    float time_difference = TimeDifference(fps_reference, measurement) / 1000000.;
    if (time_difference > 1.) {
        float processing_fps =
                static_cast<float>(processing_counter) / time_difference,
              rendering_fps =
                static_cast<float>(rendering_counter) / time_difference;

        api.BroadcastFps(processing_fps, rendering_fps);

        processing_counter = rendering_counter = 0;
        fps_reference = measurement;
    }
}

void Renderer::RenderSurface() {
    pp::Size extent = graphics->size();
    pp::ImageData image_data(handle, PP_IMAGEDATAFORMAT_RGBA_PREMUL, extent, false);

    uint8_t* surface_buffer = surface->GetBuffer();
    uint32_t* image_buffer = static_cast<uint32_t*>(image_data.data());

    uint32_t area = extent.GetArea();

    for (uint32_t i = 0; i < area; i++) {
        image_buffer[i] =
            PixelRGB(surface_buffer[i], surface_buffer[i], surface_buffer[i]);
    }

    graphics->ReplaceContents(&image_data);
    graphics->Flush(callback_factory->NewCallback(&Renderer::RenderCallback));
}

void Renderer::RenderCallback(uint32_t status) {
    render_pending = false;
}

}
