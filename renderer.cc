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

#include <exception>
#include <sstream>

#include "sys/time.h"
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

uint32_t delay(timeval& reference, uint32_t delay)
{
    timeval current;
    if (gettimeofday(&current, NULL) != 0) return 0;

    int32_t actual_delay = delay
        - (current.tv_sec - reference.tv_sec) * 1000000
        - current.tv_usec + reference.tv_usec;

    if (actual_delay > 0 && static_cast<uint32_t>(actual_delay) <= delay) {
        usleep(delay);
    }
    return delay;
}

inline uint32_t PixelRGB(const uint8_t r, const uint8_t g, const uint8_t b) {
    return 0xFF000000 | (b << 16) | (g << 8) | r;
}

class EQuit : public std::exception {};

}

namespace glow {

Renderer::Renderer(
    const pp::InstanceHandle& handle,
    Logger* logger,
    const Settings& settings,
    pp::Graphics2D* graphics)
:
   handle(handle),
   logger(logger),
   graphics(graphics),
   thread(NULL),
   surface(NULL),
   drawing(false),
   settings(settings)
{}

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

    if (callback_factory != NULL) {
        delete callback_factory;
        callback_factory = NULL;
    }

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

    callback_factory = new pp::CompletionCallbackFactory<Renderer>(this);

    timeval timestamp;
    pp::MessageLoop& message_loop = thread->message_loop();
    std::stringstream ss;
    uint32_t lost_frames = 0;

    try {
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
                if (lost_frames > 0) {
                    ss.str("");
                    ss << "Render: skipped " << lost_frames << " frames";
                    logger->Log(ss.str());
                    lost_frames = 0;
                }
                render_pending = true;
                RenderSurface();
            } else {
                lost_frames++;
            }

            delay(timestamp, 1000000 / settings.Fps());
        }
    }
    catch(EQuit) {}
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
