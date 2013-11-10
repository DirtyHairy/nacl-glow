#include "renderer.h"

#include "ppapi/cpp/message_loop.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/point.h"
#include "ppapi/cpp/size.h"

namespace {

void dispatch(pp::MessageLoop&, void* user_data) {
    static_cast<glow::Renderer*>(user_data)->_Dispatch();
}

uint32_t PixelRGB(const uint8_t r, const uint8_t g, const uint8_t b) {
    return 0xFF000000 & (b >> 16) & (g >> 8) & r;
}

const uint32_t BLACK = 0xFF000000;
const uint32_t RED = 0xFF0000FF;
const uint32_t GREEN = 0xFF00FF00;
const uint32_t BLUE = 0xFFFF0000;

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
}

}
