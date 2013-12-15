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

#include "api.h"

#include <string>

#include "ppapi/cpp/var_dictionary.h"
#include "ppapi/cpp/core.h"

#include "settings.h"
#include "instance.h"

namespace {

class EInvalidMessage {};

/**
 * The following helpers try to unwrap a value from the message. JS values are
 * represented by pp::Var instances, which we have to typecheck before
 * unwrapping.
 */
std::string MessageGetString(
    const pp::VarDictionary& msg,
    const std::string& name)
{
    if (!msg.HasKey(name)) throw EInvalidMessage();

    pp::Var value = msg.Get(name);
    if (!value.is_string()) throw EInvalidMessage();

    return value.AsString();
}

float MessageGetFloat(
    const pp::VarDictionary& msg,
    const std::string& name)
{
    if (!msg.HasKey(name)) throw EInvalidMessage();

    pp::Var value = msg.Get(name);
    if (value.is_number()) return value.AsDouble();

    throw EInvalidMessage();
}

int32_t MessageGetInt(
    const pp::VarDictionary& msg,
    const std::string& name)
{
    if (!msg.HasKey(name)) throw EInvalidMessage();

    pp::Var value = msg.Get(name);
    if (!value.is_int()) throw EInvalidMessage();

    return value.AsInt();
}

/**
 * Build an error message. The original message is returned in the
 * originalMessage field.
 */
pp::VarDictionary BuildErrorMessage(
    const std::string& description,
    const pp::Var* originalMessage = NULL)
{
    pp::VarDictionary message;

    message.Set("subject", "error");
    message.Set("message", description);
    if (originalMessage != NULL) {
        message.Set("originalMessage", *originalMessage);
    }

    return message;
}

/**
 * Build a settings message.
 */
pp::VarDictionary BuildSettingsMessage(const glow::Settings& settings) {
    pp::VarDictionary message;

    message.Set("subject", "settingsBroadcast");

    // While we can rely on the compiler to automatically generated a call to
    // the pp::Var constructor to convert our atomic value into a pp::Var, we
    // better cast it explicitly to the desired type in order to avoid nasty
    // surprises.
    message.Set("bleed",    static_cast<double>(settings.Bleed()));
    message.Set("decayLin", static_cast<int32_t>(settings.Decay_lin()));
    message.Set("decayExp", static_cast<double>(settings.Decay_exp()));
    message.Set("radius",   static_cast<int32_t>(settings.Radius()));
    message.Set("fps",      static_cast<int32_t>(settings.Fps()));

    return message;
}

/**
 * Apply a settings change request.
 */
void ApplyChangeSettingsMessage(
    const pp::VarDictionary& message,
    glow::Settings& settings)
{
    glow::Settings newSettings(settings);

    if (message.HasKey("radius")) {
        newSettings.Radius(MessageGetInt(message, "radius"));
    }
    if (message.HasKey("bleed")) {
        newSettings.Bleed(MessageGetFloat(message, "bleed"));
    }
    if (message.HasKey("decayExp")) {
        newSettings.Decay_exp(MessageGetFloat(message, "decayExp"));
    }
    if (message.HasKey("decayLin")) {
        newSettings.Decay_lin(MessageGetInt(message, "decayLin"));
    }
    if (message.HasKey("fps")) {
        newSettings.Fps(MessageGetInt(message, "fps"));
    }

    settings = newSettings;
}

}

namespace glow {

Api::Api(Instance& instance) :
    instance(instance)
{
    // Initialize the callback factory
    callback_factory = new pp::CompletionCallbackFactory<Api>(this);
}

Api::~Api() {
    delete callback_factory;
}

/**
 * All API messages consists of a JS object with a 'subject' property. Depending
 * on the subject and a payload build from the other fields.
 *
 * This method will and must be called from the main thread (as it calls
 * PostMessage directly).
 */
void Api::HandleMessage(const pp::Var& message) {
    try {
        if (!message.is_dictionary()) throw EInvalidMessage();
        // pp::VarDictionary is the data type which represents JS objects.
        pp::VarDictionary msg(message);

        std::string subject = MessageGetString(msg, "subject");
        if (subject == "requestSettings") {
            // Post a message object containing all settings to the JS side.
            // Using pp:Instance::postMessage directly implies that we must be
            // called from the main thread.
            instance.PostMessage(BuildSettingsMessage(instance.GetSettings()));

        } else if (subject == "changeSettings") {
            // Try to unwrap all modified settings from the message and apply
            // them to the settings object.
            ApplyChangeSettingsMessage(msg, instance.GetSettings());

        } else {
            throw EInvalidMessage();
        }

    } catch (EInvalidMessage& e) {
        // Invalid messages are returned to the sender.
        instance.PostMessage(BuildErrorMessage("invalid message", &message));
    }
}

/**
 * Broadcast the current FPS to the JS side. This will be called from the
 * renderer thread and thus we cannot call PostMessage directly but have to
 * schedule a callback on the main thread instead.
 */
void Api::BroadcastFps(float processing_fps, float rendering_fps) {
    pp::VarDictionary msg;

    msg.Set("subject", "fpsBroadcast");
    msg.Set("processingFps", static_cast<double>(processing_fps));
    msg.Set("renderingFps", static_cast<double>(rendering_fps));

    // pp::Core::CallOnMainThread schedules a callback on the main thread's
    // message loop.
    pp::Module::Get()->core()->CallOnMainThread(0,
        callback_factory->NewCallback(&Api::DoPostMessage, msg));
}

/*
 * Call PostMessage on the main thread.
 */
void Api::DoPostMessage(uint32_t result, const pp::Var& message) {
    if (result == PP_OK) instance.PostMessage(message);
}

}
