/**
 * Mikey module.
 *
 * An EventMitter for (Apple) Mikey HID events (working only on Mac OS X).
 * 
 * Usage:
 * ````JavaScript
 * var mikey = require("mikey");
 * mikey.on("playPause", function onPlayPause() { ... });
 * ````
 *
 * Events:
 * `"playPause"`
 * 
 * NOTE: loading the module taps all Mikey events at system (OS) level,
 * excluding all the other applications.
 */

"use strict";

var mikey = require("bindings")("mikey");
var EventEmitter = require("events").EventEmitter;

var emitter = new EventEmitter();
/* @param {String} keyEvent - for testing purposes */
emitter.sendKeyEvent = mikey.sendKeyEvent;

var mikeyListener = function mikeyListener(keyEvent) {
  emitter.emit(keyEvent);
}
mikey.setListener(mikeyListener);

module.exports = emitter;
