"use strict";

var mikey = require("./index");

var testPassed = false;

var playPauseListener = function playPauseListener () {
  console.log("[playPauseListener]", "keyEvent:", "playPause");
  testPassed = true;
};

var pressPlayPause = function pressPlayPause () {
  mikey.sendKeyEvent("playPause");
};

console.log("TEST STARTED");

mikey.addListener("playPause", playPauseListener);

var seconds = 3;
console.log("(you have " + seconds + " seconds to press the microphone button)");

console.log("emulating playPause press -- microphone button is not working yet");
setTimeout(pressPlayPause, 1000);

setTimeout(function () {
  console.log("TEST", testPassed ? "PASSED" : "FAILED");
}, seconds*1000);
