#include <string>
#include <sstream>

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/input_event.h"

#include "logger.h"
#include "renderer.h"

namespace glow {

class Instance : public pp::Instance {
    public:

        explicit Instance(PP_Instance instance) :
            pp::Instance(instance),
            graphics(NULL),
            renderer(NULL),
            drawing(false)
        {
            logger = new Logger(this);
            RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE);
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

            drawing = false;
        }

        virtual bool HandleInputEvent(const pp::InputEvent& event) {
            if (renderer == NULL) return false;

            switch (event.GetType()) {
                case PP_INPUTEVENT_TYPE_MOUSEDOWN:
                    drawing = true;
                    renderer->MoveTo(pp::MouseInputEvent(event).GetPosition());
                    return true;

                case PP_INPUTEVENT_TYPE_MOUSELEAVE:
                case PP_INPUTEVENT_TYPE_MOUSEUP:
                    drawing = false;
                    return true;

                case PP_INPUTEVENT_TYPE_MOUSEMOVE:
                    if (drawing) {
                        renderer->DrawTo(pp::MouseInputEvent(event).GetPosition());
                    }
                    return true;

                default:
                    break;
            }

            return false;
        }

    private:

        pp::Graphics2D* graphics;
        Logger* logger;
        Renderer* renderer;
        bool drawing;
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
