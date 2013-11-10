#ifndef GLOW_RENDERER_H
#define GLOW_RENDERER_H

#include "ppapi/utility/threading/simple_thread.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/instance_handle.h"

#include "logger.h"

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
        pp::Graphics2D* graphics;
        pp::SimpleThread* thread;
        Logger* logger;

        Renderer(const Renderer&);
        const Renderer& operator=(const Renderer&);
};

}

#endif // GLOW_RENDERER_H
