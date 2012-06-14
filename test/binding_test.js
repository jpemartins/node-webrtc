console.log("using node "+process.version);

// BEGIN NODE
var webrtc = require("../webrtc")
  , colors = require("colors");
// END NODE
//
var PeerConnection = typeof (window) !== "undefined" ? (webkitPeerConnection || mozPeerConnection || window.PeerConnection) : webrtc.PeerConnection;

console.log("dumping <WebRTC> native module:".blue);
console.dir(webrtc);

console.log("dumping <PeerConnection> object:".blue);
console.dir(peerconnection = new PeerConnection("STUN stun.l.google.com:19302"));

peerconnection.onconnecting = function () {
	console.log("onconnecting".green);
	console.log((state+"").yellow);
}

peerconnection.onopen = function () {
	console.log("onopen".green);
}

peerconnection.onerror = function (err) {
	console.log("onerror".green);
	console.log(err.yellow)
}

peerconnection.onclose = function () {
	console.log("onclose".green);
}

peerconnection.onaddstream = function (stream) {
	console.log("onaddstream".green);
}

peerconnection.onremovestream = function () {
	console.log("onremovestream".green);
}

peerconnection.onmessage = function (message) {
}

peerconnection.onstatechange = function (state) {
	console.log("onstatechange="+state.green);
}

peerconnection.onsignalingmessage = function (signal) {
	console.log("onsignalingmessage".green);
	console.log(signal.yellow);
}

console.log("PeerConnection.addStream():".grey + peerconnection.addStream("audio_label"));
//console.log("PeerConnection.close():".grey + peerconnection.close());


/* XXX - JUST TO HOLD THE EVENT LOOP */
require('http').createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('\n');
}).listen(1337, "127.0.0.1");


setInterval(function () {
	webrtc.render();
}, 20);