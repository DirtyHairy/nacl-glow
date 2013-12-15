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

#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/point.h"
#include "ppapi/cpp/size.h"
#include "ppapi/cpp/message_loop.h"

namespace {

int32_t TimeDifference(const timeval& t1, const timeval& t2) {
    return (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
}

/**
 * Sleep until a certain delay w.r.t. a reference timestamp has passed. Note
 * the use of the POSIX calls gettimeofday and usleep.
 */
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
    const volatile Settings& settings,
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
    // Create the callback factory. According to the API docs, creating and
    // destroying the callback factory is not threadsafe, while genrating
    // callbacks is (unless differently specified via trait), and creation
    // and destruction should happen from the same thread that executes the
    // callbacks in order to avoid races. However, the thread will not be
    // dispatched before this function has finished, so we are safe.
    callback_factory = new pp::CompletionCallbackFactory<Renderer>(this);
}

Renderer::~Renderer() {
    Stop();

    // See above. Stop() waits until the message loop has been closed and
    // the thread has terminated, so there is no danger of races.
    delete callback_factory;
}

void Renderer::DispatchThreadCallback(pp::MessageLoop&, void* user_data) {
    // Dispatch the main loop.
    static_cast<glow::Renderer*>(user_data)->Dispatch();
}

void Renderer::Start() {
    if (thread != NULL) {
        return;
    }

    // Create a new thread and dispatch it.
    thread = new pp::SimpleThread(handle);
    thread->StartWithFunction(&DispatchThreadCallback, this);
}

void Renderer::Stop() {
    if (thread == NULL) {
        return;
    }

    // SimpleThread::Join posts a quit message to the message loop, closes it
    // and waits for the threads to join. Closing the message loop will cause
    // the main loop to terminate, after which the threads join and the thread
    // and surface instances are destroyed.
    thread->Join();
    delete thread;
    thread = NULL;

    delete surface;
}

/**
 * As the three render API calls are called from the main thread, they do not
 * actually do any work, but post callbacks to the message loop instead. The
 * main loop polls and runs the message loop, causing the requested actions
 * to be eventually executed, while the API call returns immediatelly after
 * queuing the message.
 */
void Renderer::MoveTo(const pp::Point& x) {
    // Generate a callback from the factory. Note how the factory binds the
    // value of x.
    thread->message_loop().PostWork(callback_factory->NewCallback(
        &Renderer::DoMoveTo, x)
    );
}

/**
 * See above.
 */
void Renderer::DrawTo(const pp::Point& x) {
    thread->message_loop().PostWork(callback_factory->NewCallback(
        &Renderer::DoDrawTo, x)
    );
}

/**
 * See above.
 */
void Renderer::SetDrawing(bool isDrawing) {
    thread->message_loop().PostWork(callback_factory->NewCallback(
        &Renderer::DoSetDrawing, isDrawing)
    );
}

/**
 * The three worker callbacks are dispatched on the rendering thread and do
 * the actual work.
 */
void Renderer::DoMoveTo(uint32_t status, const pp::Point& x) {
    if (status == PP_OK) current_position = x;
}

void Renderer::DoDrawTo(uint32_t status, const pp::Point& x) {
    if (status == PP_OK && surface != NULL) surface->Line(
        current_position.x(), current_position.y(),
        x.x(), x.y(), settings.Radius()
    );
    current_position = x;
}

void Renderer::DoSetDrawing(uint32_t status, bool isDrawing) {
    if (status == PP_OK) drawing = isDrawing;
}

/**
 * The main loop.
 */
void Renderer::Dispatch() {
    pp::Size extent = graphics->size();
    surface = new Surface(extent.width(), extent.height());

    timeval timestamp, fps_reference;
    // pp::SimpleThread automatically creates a pp::MessageLoop and associates
    // it with the thread -> get it.
    pp::MessageLoop& message_loop = thread->message_loop();
    uint32_t render_counter = 0, processing_counter = 0;

    try {
        // Initialize the reference timestamp for FPS calculation
        if (gettimeofday(&fps_reference, NULL) != 0) throw EQuit();

        // Broadcast the reference FPS as initial value
        api.BroadcastFps(settings.Fps(), settings.Fps());

        while (true) {
            if (gettimeofday(&timestamp, NULL) != 0) throw EQuit();

            surface->Decay(
                settings.Bleed(),
                settings.Decay_factor(),
                settings.Decay_lin()
            );

            // Calling PostQuit(false) posts a quit message without closing the
            // loop. This ensures that Run() returns after all messages have
            // been processed, effectively polling the loop.
            //
            // Once the loop has been closed for good, those two will fail,
            // causing the main loop to terminate.
            if (message_loop.PostQuit(false) != PP_OK) throw EQuit();
            if (message_loop.Run() != PP_OK) throw EQuit();

            if (drawing){
                surface->Circle(
                    current_position.x(),
                    current_position.y(),
                    settings.Radius()
                );
            }

            // Checking whether the previous render request has completed
            // before rendering avoid unnecessary work and allows us to count
            // the rendering FPS separately from the processing FPS.
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

/**
 * Each second we calculate calculate the FPS rendered and processed and
 * broadcast them via the API.
 */
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

/**
 * Copy the surface data to the graphics context.
 */
void Renderer::RenderSurface() {
    pp::Size extent = graphics->size();

    // Aquire an image data buffer from pepper. According to the docs, the
    // buffers are cached internally and reused.
    pp::ImageData image_data(handle, PP_IMAGEDATAFORMAT_RGBA_PREMUL, extent, false);

    uint8_t* surface_buffer = surface->GetBuffer();
    uint32_t* image_buffer = static_cast<uint32_t*>(image_data.data());

    uint32_t area = extent.GetArea();

    // Copy the surface data to the buffer
    for (uint32_t i = 0; i < area; i++) {
        image_buffer[i] =
            PixelRGB(surface_buffer[i], surface_buffer[i], surface_buffer[i]);
    }

    // Calling ReplaceContents replaces the buffer of the graphics context with
    // our freshly populated buffer. The previously bound buffer is freed and
    // will be recycled in our next iteration. This is how the pepper docs
    // advise for implementing double buffering :)
    graphics->ReplaceContents(&image_data);

    // ReplaceContents only queues the operation, we need to call Flush in order
    // to actually dispatch it. Flush returns immediatelly, and the callback is
    // executed when the operation has actually completed. Passing an empty
    // callback would block the thread until the operation has completed and
    // enforce synchrouneous operation. However, keeping it async allows us to
    // process at a constant frame rate even if rendering is too slow.
    graphics->Flush(callback_factory->NewCallback(&Renderer::RenderCallback));
}

/**
 * The callback just lowers the render_pending flag, clearing the way for a new
 * render operation.
 */
void Renderer::RenderCallback(uint32_t status) {
    render_pending = false;
}

}
