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

#include "instance.h"

namespace glow {

Instance::Instance(PP_Instance instance) :
    pp::Instance(instance),
    graphics(NULL),
    renderer(NULL),
    drawing(false)
{
    logger = new Logger(*this);
    api = new Api(*this);

    // This method must be called in order to subscribe to different
    // classes of input events.
    RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE);
}

Instance::~Instance() {
    if (graphics != NULL) delete graphics;
    if (renderer != NULL) delete renderer;
    if (api != NULL) delete api;
}

/**
 * We use DidChangeView in order to create a graphics context and an instance
 * of our renderer class when the module becomes visible for the first time
 */
void Instance::DidChangeView(const pp::View& view) {
    pp::Size extent = view.GetRect().size();

    if (graphics == NULL) {
        // In order to display anything we must create a graphics context and
        // bind it.
        graphics = new pp::Graphics2D(this, extent, true);
        BindGraphics(*graphics);

        // The renderer runs in a separate thread and houses the main loop.
        renderer = new Renderer(this, *logger, *api, settings, graphics);
        renderer->Start();
    }

    drawing = false;
}

/**
 * The renderer implements turtle-like graphics primitives, MoveTo and DrawTo.
 * In addition, a circle is drawn at the current curser position if the mouse
 * button is pressed.
 */
bool Instance::HandleInputEvent(const pp::InputEvent& event) {
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

/**
 * Messages are delegated to the API object.
 */
void Instance::HandleMessage(const pp::Var& message) {
    api->HandleMessage(message);
}

}
