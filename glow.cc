#include <string>
#include <sstream>

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/size.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/instance_handle.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/point.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/view.h"
#include "ppapi/utility/completion_callback_factory.h"

namespace {
    uint32_t PixelRGB(const uint8_t r, const uint8_t g, const uint8_t b) {
        return 0xFF000000 & (b >> 16) & (g >> 8) & r;
    }

    const BLACK = 0xFF000000;
    const RED = 0xFF0000FF;
    const GREEN = 0xFF00FF00;
    const BLUE = 0xFFFF0000;
}

namespace glow {

    class Instance : public pp::Instance {
        public:

            explicit Instance(PP_Instance instance) :
                pp::Instance(instance),
                handle(instance),
                graphics(NULL)
            {
                callback_factory = new pp::CompletionCallbackFactory<Instance>(this);
            }

            ~Instance() {
                delete callback_factory;
                if (graphics != NULL) {
                    delete graphics;
                }
            }

            virtual void DidChangeView(const pp::View& view) {
                pp::Size extent = view.GetRect().size();

                if (graphics == NULL) {
                    graphics = new pp::Graphics2D(this, extent, true);
                    BindGraphics(*graphics);
                }
                
                pp::ImageData image_data(this, PP_IMAGEDATAFORMAT_RGBA_PREMUL, extent, false);
                uint32_t* buffer = static_cast<uint32_t*>(image_data.data());

                for (int i = 0; i < extent.GetArea(); i++) {
                    buffer[i] = PixelRGB(0, 255, 0);
                }

                graphics->PaintImageData(image_data, pp::Point(0, 0));
                graphics->ReplaceContents(&image_data);

                graphics->Flush(callback_factory->NewCallback(&Instance::OnPaint));
            }

        private:

            pp::InstanceHandle handle;
            pp::Graphics2D* graphics;
            pp::CompletionCallbackFactory<Instance>* callback_factory;

            void Log(const std::string& message) {
                LogToConsole(PP_LOGLEVEL_LOG, pp::Var(message));
            }

            void OnPaint(int32_t result) {
                std::stringstream ss;
                ss.str("");
                ss << "flush complete: " << (int)result;
                Log(ss.str());
            }

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
