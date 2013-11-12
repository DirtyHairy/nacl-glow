#ifndef GLOW_RENDERER_H
#define GLOW_RENDERER_H

#include "ppapi/utility/threading/simple_thread.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/instance_handle.h"

#include "logger.h"
#include "grayscale_buffer.h"

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

        void _Dispatch();

    private:
   
        pp::InstanceHandle handle;
        Logger* logger;
        pp::Graphics2D* graphics;
        pp::SimpleThread* thread;
        pp::CompletionCallbackFactory<Renderer>* callback_factory;
        GrayscaleBuffer* grayscale_buffer;
        bool render_pending;

        void RenderBuffer();
        void DecayBuffer();
        void RenderCallback(uint32_t status);

        Renderer(const Renderer&);
        const Renderer& operator=(const Renderer&);
};

}

#endif // GLOW_RENDERER_H
