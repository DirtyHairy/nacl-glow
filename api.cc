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

#include "settings.h"
#include "instance.h"

namespace {

class EInvalidMessage {};

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

pp::VarDictionary BuildSettingsMessage(const glow::Settings& settings) {
    pp::VarDictionary message;

    message.Set("subject", "settingsBroadcast");
    message.Set("bleed",    static_cast<double>(settings.Bleed()));
    message.Set("decayLin", static_cast<int32_t>(settings.Decay_lin()));
    message.Set("decayExp", static_cast<double>(settings.Decay_exp()));
    message.Set("radius",   static_cast<int32_t>(settings.Radius()));
    message.Set("fps",      static_cast<int32_t>(settings.Fps()));

    return message;
}

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

Api::Api(Instance& instance) : instance(instance) {}

void Api::HandleMessage(const pp::Var& message) {
    try {
        if (!message.is_dictionary()) throw EInvalidMessage();
        pp::VarDictionary msg(message);

        std::string subject = MessageGetString(msg, "subject");
        if (subject == "requestSettings") {
            instance.PostMessage(BuildSettingsMessage(instance.GetSettings()));

        } else if (subject == "changeSettings") {
            ApplyChangeSettingsMessage(msg, instance.GetSettings());

        } else {
            throw EInvalidMessage();
        }

    } catch (EInvalidMessage& e) {
        instance.PostMessage(BuildErrorMessage("invalid message", &message));
    }
}

}
