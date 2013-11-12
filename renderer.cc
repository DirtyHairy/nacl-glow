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
    pp::Graphics2D* graphics)
:
   handle(handle),
   logger(logger),
   graphics(graphics),
   thread(NULL),
   surface(NULL)
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

void Renderer::_Dispatch() {
    pp::Size extent = graphics->size();
    surface = new Surface(extent.width(), extent.height());

    for (int x = 0; x < 100; x++)
        for (int y = 0; y < 100; y++)
            surface->Set(x, y, 200);

    timeval timestamp;
    pp::MessageLoop& message_loop = thread->message_loop();
    std::stringstream ss;
    uint32_t lost_frames = 0;

    try {
        while (true) {
            if (gettimeofday(&timestamp, NULL) != 0) throw EQuit();

            surface->Decay();

            if (message_loop.PostQuit(false) != PP_OK) throw EQuit();
            if (message_loop.Run() != PP_OK) throw EQuit();

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

            delay(timestamp, 1000000 / 30);
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
