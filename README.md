1. Mikey

Mikey is a simple Node.js module for accessing inline microphone controls.

Mikey exposes an `EventEmitter` API to listen to inline microphone controls events.

Example usage:

````JavaScript
var mikey = require('mikey');
 
mikey.addListener('playPause', function(){
  // button on headset has been pressed
});
````

Currently Mikey works only on Mac OS X.

NOTE: when the module is loaded, it captures all Mikey events at system (OS) level,
excluding all the other applications from receiving the events.
