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

        void RenderSurface();
        void RenderCallback(uint32_t status);

        void DoMoveTo(uint32_t status, const pp::Point& x);
        void DoDrawTo(uint32_t status, const pp::Point& x);

        Renderer(const Renderer&);
        const Renderer& operator=(const Renderer&);
};

}

#endif // GLOW_RENDERER_H
