// BEGIN NODE
var webrtc = require("./webrtc")
  , colors = require("colors");
// END NODE
//
var peerconnection;

console.log("dumping <WebRTC> native module:".blue);
console.dir(webrtc);

console.log("dumping <PeerConnection> object:".blue);
console.dir(peerconnection = new webrtc.PeerConnection("STUN stun.l.google.com:19302"));

console.log("<PeerConnection> methods invoke:".grey);
console.log("PeerConnection.addStream():".grey + peerconnection.addStream("audio_label"));
console.log("PeerConnection.connect():".grey + peerconnection.connect());
/*
console.log(peerconnection.addStream());
console.log(peerconnection.send());
console.log(peerconnection.close());
*/


/* JUST TO HOLD THE EVENT LOOP */
require('http').createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('\n');
}).listen(1337, "127.0.0.1");
