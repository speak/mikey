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
 * `"next"`
 * `"prev"`
 * `"soundUp"`
 * `"soundDown"`
 * 
 * NOTE: loading the module taps all Mikey events at system (OS) level,
 * excluding all the other applications.
 */

"use strict";

var mikey = require("bindings")("mikey");
var EventEmitter = require("events").EventEmitter;

var keyEvents = {
  0x89: "playPause",
  0x8A: "next",
  0x8B: "prev",
  0x8C: "soundUp",
  0x8D: "soundDown"
};

var usageCodes = {};
for (var k in keyEvents) { usageCodes[keyEvents[k]] = k; }

var emitter = new EventEmitter();

/* @param {String} event - for testing purposes */
emitter.sendKeyEvent = function (event) {
  mikey.sendKeyEvent(usageCodes[event]);
};

var mikeyListener = function mikeyListener(usageCode) {
  var keyEvent = keyEvents[usageCode];
  emitter.emit(keyEvent) || mikey.sendBackKeyEvent(usageCode);
}
mikey.setListener(mikeyListener);

module.exports = emitter;
