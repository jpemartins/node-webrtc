WebRTC in Node.JS
=========================

Google WebRTC stack in Node.JS. 

To implement the Media Processing API is in my future work.

Highly unstable. 

Building
========

First you must compile Google WebRTC stack. See [official page](http://www.webrtc.org/reference/getting-started) for more information.
After that, you must point it to WEBRTC_ROOT_PATH. REVISION refers to the latest tested revision of Google WebRTC.

	export WEBRTC_ROOT_PATH=/path/to/webrtc/trunk
	make
	npm install

Testing
=======

Assuming your browser will be the "caller", run the following commands and go [here](http://127.0.0.1:9999/peerconnection.html)

	node test/server.js
	node test/peerconnection.js --callee
