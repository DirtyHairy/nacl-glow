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

(function(scope) {
    var loader, module, controls,
        /**
         * This object will house the input sliders --- we'll fetch them later
         * by ID
         */
        inputs = {
            bleed: 'bleed',
            radius: 'radius',
            decayExp: 'decay_exp',
            decayLin: 'decay_lin',
            fps: 'target_fps'
        },
        /**
         * Dito, this houses the FPS displays.
         */
        fpsDisplays = {
            processingFps: 'fps_processing',
            renderingFps: 'fps_rendering'
        },
        startButton, stopButton;

    /**
     * Prepare a value for sending it to the module by ensuring the correct
     * type.
     */
    function marshall(name, value) {
        switch (name) {
            case 'bleed':
                return parseFloat(value);
            case 'radius':
                return parseInt(value, 10);
            case 'decayExp':
                return parseFloat(value);
            case 'decayLin':
                return parseInt(value, 10);
            case 'fps':
                return parseInt(value, 10);
            default:
                return value;
        }
    }

    /**
     * Handle an incoming message and delegate to an event handler.
     */
    function onMessage(message) {
        var subject = message.data.subject;

        if (subject == 'settingsBroadcast') {
            onSettingsBroadcast(message.data);
        } else if (subject == 'fpsBroadcast') {
            onFpsBroadcast(message.data);
        } else if (subject == 'error') {
            console.log('module reports error: ', message.data);
        } else {
            console.log(message.data);
        }
    }
    
    /**
     * Update FPS display.
     */
    function onFpsBroadcast(message) {
        for (name in fpsDisplays) {
            if (message[name]) {
                getFpsMonitor(fpsDisplays[name]).innerHTML =
                    message[name].toFixed(3);
            }
        }
    }

    /**
     * Update sliders with updates settings.
     */
    function onSettingsBroadcast(message) {
        for (var name in inputs) (function(name) {
            var container = inputs[name],
                input = getInput(container);

            if (message.hasOwnProperty(name)) {
                input.value = message[name];
                onInputChange(name);
            }
        })(name);

        // The sliders are initially hidden, so make sure that they are visible.
        controls.style.visibility = 'visible';
    }

    /**
     * Relay an input change to the module.
     */
    function onInputChange(name, relay) {
        var container = inputs[name],
            input = getInput(container),
            monitor = getMonitor(container);

        monitor.innerHTML = input.value;
        if (relay) {
            relaySettings();
        }
    }


    function onStartButtonClick() {
        if (module) {
            module.postMessage({
                subject: 'start'
            });
        }
    }

    function onStopButtonClick() {
        if (module) {
            module.postMessage({
                subject: 'stop'
            });
        }
    }

    /**
     * Build a settings update message and post it to the module
     */
    function relaySettings() {
        var message = {
            subject: 'changeSettings'
        }

        for (var name in inputs) {
            message[name] = marshall(name, getInput(inputs[name]).value);
        }

        if (module) {
            module.postMessage(message);
        }
    }

    /**
     * Get the actual input sliders and data containers from their parent
     * elements.
     */
    function getInput(container) {
        return container.getElementsByTagName('input')[0];
    }

    function getMonitor(container) {
        return container.getElementsByTagName('span')[0];
    }

    function getFpsMonitor(container) {
        return container.getElementsByTagName('span')[0];
    }

    /**
     * Bootstrap.
     */
    function onDocumentLoad() {
        loader = document.getElementById('loader');
        module = document.getElementById('glow_module');
        controls = document.getElementById('controls');

        // Retrieve the input elements.
        for (var name in inputs) (function(name) {
            var container = inputs[name] = document.getElementById(inputs[name]),
                input = getInput(container);

            input.addEventListener('change', function() {
                onInputChange(name, true);
            }, true);
        })(name);

        // Retrieve FPS display elements.
        for (var name in fpsDisplays) {
            fpsDisplays[name] = document.getElementById(fpsDisplays[name]);
        }

        startButton = document.getElementById('startbutton');
        stopButton = document.getElementById('stopbutton');
        startButton.addEventListener('click', onStartButtonClick);
        stopButton.addEventListener('click', onStopButtonClick);

        // Export the module load handler.
        scope.onModuleLoad = onModuleLoad;

        // Handle the race between module and document load by calling the
        // module load handler directly if the module has already loaded.
        if (scope.moduleLoaded) onModuleLoad();
    }


    /**
     * Hide the loading message and request an initial settings update once the
     * module has loaded.
     */
    function onModuleLoad() {
        loader.style.display = 'none';
        module.addEventListener('message', onMessage, true);
        module.postMessage({
            subject: 'requestSettings'
        });
    }

    // Export the document load handler to the global scope.
    scope.onDocumentLoad = onDocumentLoad;
})(window);
