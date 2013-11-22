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

#include <string>
#include <sstream>

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/var_dictionary.h"

#include "logger.h"
#include "renderer.h"
#include "settings.h"

namespace glow {

class Instance : public pp::Instance {
    public:

        explicit Instance(PP_Instance instance) :
            pp::Instance(instance),
            graphics(NULL),
            renderer(NULL),
            drawing(false)
        {
            logger = new Logger(*this);
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

                renderer = new Renderer(this, logger, settings, graphics);
                renderer->Start();
            }

            drawing = false;
        }

        virtual bool HandleInputEvent(const pp::InputEvent& event) {
            if (renderer == NULL) return false;

            switch (event.GetType()) {
                case PP_INPUTEVENT_TYPE_MOUSEDOWN:
                    drawing = true;
                    renderer->SetDrawing(true);
                    renderer->MoveTo(pp::MouseInputEvent(event).GetPosition());
                    return true;

                case PP_INPUTEVENT_TYPE_MOUSELEAVE:
                case PP_INPUTEVENT_TYPE_MOUSEUP:
                    drawing = false;
                    renderer->SetDrawing(false);
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

        virtual void HandleMessage(const pp::Var& message) {
            try {
                if (!message.is_dictionary()) throw EInvalidMessage();
                pp::VarDictionary msg(message);

                std::string subject = MessageGetString(msg, "subject");
                if (subject == "requestSettings") {
                    PostMessage(BuildSettingsMessage());
                } else {
                    throw EInvalidMessage();
                }

            } catch (EInvalidMessage& e) {
                PostMessage(BuildErrorMessage("invalid message", &message));
            }
        }

    private:

        class EInvalidMessage {};

        pp::Graphics2D* graphics;
        Logger* logger;
        Renderer* renderer;
        bool drawing;
        Settings settings;

        std::string MessageGetString(
            const pp::VarDictionary& msg,
            const std::string& name
        ) const {
            if (!msg.HasKey(name)) throw EInvalidMessage();

            pp::Var value = msg.Get(name);
            if (!value.is_string()) throw EInvalidMessage();

            return value.AsString();
        }

        pp::VarDictionary BuildErrorMessage(
            const std::string& description,
            const pp::Var* originalMessage = NULL
        ) const
        {
            pp::VarDictionary message;
    
            message.Set("subject", "error");
            message.Set("message", description);
            if (originalMessage != NULL) {
                message.Set("originalMessage", *originalMessage);
            }

            return message;
        }
    
        pp::VarDictionary BuildSettingsMessage() const {
            pp::VarDictionary message;
    
            message.Set("subject", "settingsBroadcast");
            message.Set("bleed",    static_cast<double>(settings.Bleed()));
            message.Set("decayExp", static_cast<int32_t>(settings.Decay_lin()));
            message.Set("decayLin", static_cast<double>(settings.Decay_exp()));
            message.Set("radius",   static_cast<int32_t>(settings.Radius()));
            message.Set("fps",      static_cast<int32_t>(settings.Fps()));
    
            return message;
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
