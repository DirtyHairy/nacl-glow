#include <string>
#include <sstream>

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

#include "logger.h"
#include "renderer.h"

namespace glow {

class Instance : public pp::Instance {
    public:

        explicit Instance(PP_Instance instance) :
            pp::Instance(instance),
            graphics(NULL),
            renderer(NULL)
        {
            logger = new Logger(this);
        }

        ~Instance() {
            if (graphics != NULL) delete graphics;
            if (renderer != NULL) delete renderer;
        }

        virtual void DidChangeView(const pp::View& view) {
            pp::Size extent = view.GetRect().size();

            if (graphics == NULL) {
                graphics = new pp::Graphics2D(this, extent, true);
                BindGraphics(*graphics);

                renderer = new Renderer(this, logger, graphics);
                renderer->Start();
            }
        }

    private:

        pp::Graphics2D* graphics;
        Logger* logger;
        Renderer* renderer;
};

class Module : public pp::Module {
    public:
    
        Module() : pp::Module() {}

        ~Module() {}

        pp::Instance* CreateInstance(PP_Instance instance) {
            return new Instance(instance);
        }
};

}

namespace pp {

Module* CreateModule() {
    return new glow::Module();
}

}
