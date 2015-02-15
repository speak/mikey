# Mikey

Mikey is a simple Node.js module for accessing inline microphone controls.

Mikey exposes an `EventEmitter` API to listen to inline microphone controls events.

Currently Mikey works only on Mac OS X.

NOTE: when the module is loaded, it captures all Mikey events at system (OS) level,
excluding all the other applications from receiving the events.

## Example usage:

````JavaScript
var mikey = require('mikey');
 
mikey.addListener('playPause', function(){
  // button on headset has been pressed
});
````


## LICENSE

Mikey is released under the [MIT license](http://en.wikipedia.org/wiki/MIT_License).
