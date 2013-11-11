#include "renderer.h"

#include <exception>

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

void delay(timeval& reference, uint32_t delay)
{
    timeval current;
    if (gettimeofday(&current, NULL) != 0) return;

    int32_t actual_delay = delay
        - (current.tv_sec - reference.tv_sec) * 1000000
        - current.tv_usec + reference.tv_usec;

    if (actual_delay > 0) usleep(delay);
}

uint32_t PixelRGB(const uint8_t r, const uint8_t g, const uint8_t b) {
    return 0xFF000000 & (b >> 16) & (g >> 8) & r;
}

const uint32_t BLACK = 0xFF000000;
const uint32_t RED = 0xFF0000FF;
const uint32_t GREEN = 0xFF00FF00;
const uint32_t BLUE = 0xFFFF0000;

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
   thread(NULL)
{}

Renderer::~Renderer() {
    Stop();

    logger->Log("finito");
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
}

void Renderer::_Dispatch() {
    pp::Size extent = graphics->size();
    pp::ImageData image_data(handle, PP_IMAGEDATAFORMAT_RGBA_PREMUL, extent, false);
    uint32_t* buffer = static_cast<uint32_t*>(image_data.data());

    for (int i = 0; i < extent.GetArea(); i++) {
        buffer[i] = RED;
    }

    graphics->PaintImageData(image_data, pp::Point(0, 0));
    graphics->ReplaceContents(&image_data);

    graphics->Flush(pp::CompletionCallback());

    logger->Log("Clear OK");

    timeval timestamp;
    pp::MessageLoop& message_loop = thread->message_loop();

    try {
        while (true) {
            if (gettimeofday(&timestamp, NULL) != 0) throw EQuit();

            logger->Log("tick");

            if (message_loop.PostQuit(false) != PP_OK) throw EQuit();
            if (message_loop.Run() != PP_OK) throw EQuit();

            delay(timestamp, 500000);
        }
    }
    catch(EQuit) {}
}

}
