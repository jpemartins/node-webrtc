var env = {
   node: typeof(module) !== "undefined"
 , browser: typeof(window) !== "undefined"

 , caller: true
 , capture: "audio,video"

 , argv: {}
}

// NODE MODULES
var webrtc;

if (env.node) {
  webrtc = require("../webrtc");
  global.WebSocket = require('ws');

  env.argv = require('optimist').argv;
  env.caller = !env.argv.callee;
  env.capture = env.argv.capture || "audio,video"
}

if (env.browser) {
	window.location.search.substr(1)
			.split("&")
			.map(function (value) {
				if (value.length == 0) {
					return
				}

				var p = value.split("=");
				env.argv[p[0]] = p[1] || true;
				env[p[0]] = env.argv[p[0]];
			});

	if (env.callee) {
		env.caller = false;
	}
}

console.log("Arguments: ", env.argv);
console.log("Environment: caller=%s, capture=%s, browser=%s, node=%s", env.caller, env.capture, env.browser, env.node);

function MessageQueue (url) {
    this.wsUrl = url || "ws://127.0.0.1:9999/";
    
    this.queue = [];
    this.ready = false;
}

MessageQueue.prototype.connect = function () {
    this.socket = new WebSocket(this.wsUrl);
    this.socket.onopen = this.onopen;
    this.socket.onclose = this.onclose;
    this.socket.onerror = this.onerror;    

    var self = this;
    this.socket.onmessage = function (event) {
        self.onmessage(event);
    }
}

MessageQueue.prototype.onopen = function () {
    console.info("queue success");
    this.ready = true;
}

MessageQueue.prototype.onclose = function () {
    console.warn("queue closed");   
}

MessageQueue.prototype.onerror = function () {
    console.error("queue error");      
}

MessageQueue.prototype.onmessage = function (event) {}

MessageQueue.prototype.send = function (signal) {
    this.queue.push(signal);
    this.socket.send(signal);
}

MessageQueue.prototype.close = function () {
    this.socket.close();
}

var peerConfig = "STUN stun.l.google.com:19302";
function Peer (onsignalingmessage) {
    console.warn(peerConfig);

    if (env.browser) {      
      this.connection = new webkitDeprecatedPeerConnection(peerConfig, onsignalingmessage);
    } else if (env.node) {
      this.connection = new webrtc.PeerConnection(peerConfig);
      this.connection.onsignalingmessage = onsignalingmessage;
    }

    var self = this;
    this.connection.onopen = function () { self.onopen };
    this.connection.onconnection = function () { self.onconnection };
    this.connection.onaddstream = function (event) { self.onaddstream(event) };
    this.connection.onremovestream = function (event) { self.onremovestream(event) };
    this.connection.onclose = function () { self.onclose };
    this.connection.onerror = function (err) { self.onerror(err) };
}

Peer.prototype = {  
    onopen: function () {
    console.info("OPENED");
  }

  , onconnection: function () {
    console.warn("CONNECTING");
  }

  , onaddstream: function (event) {
    console.info("STREAM ADDED label=", event);
  }

  , onremovestream: function () {
    console.warn("STREAM REMOVED");
  }

  , onclose: function () {
    console.warn("CLOSED");
  }

  , onerror: function (err) {
    console.warn("ERROR", err);
  }

  , onmessage: function (message) {
    console.warn("MESSAGE: ", message);
  }

  , addStream: function (stream) {
    this.connection.addStream(stream);
  }

  , processSignalingMessage: function (message) {
    this.connection.processSignalingMessage(message);
  }
};

/**
  * Browser-UI
  */
if (env.browser) {
  var UI = {
    
      remoteVideo: document.querySelector("#remote > video")
    , localVideo: document.querySelector("#user > video")
    
    , errorLabel: document.getElementById("error")
    , successLabel: document.getElementById("success")

    , hide: function (element) {
        element.classList.add("hidden");
      }

    , show: function (element) {
        element.classList.remove("hidden");        
      }

    , showLocalVideo: function (stream) {
        this.show(this.successLabel);    
        this.localVideo.src = webkitURL.createObjectURL(stream);
    }

    , showRemoteVideo: function (stream) {
        this.remoteVideo.src = webkitURL.createObjectURL(stream);
    }
  };
}

/**
  * Runtime
  */
var pc;
var queue = new MessageQueue();

/**
  * Make a "call"
  */
function call(localStream) {
    !!UI && UI.showLocalVideo(localStream);

    pc = new Peer(function (message) {
      var signal = JSON.parse(env.browser ? message.substr(3) : message);
      queue.send(JSON.stringify(signal));
    });

    pc.onaddstream = function (event) {
      !!UI && UI.showRemoteVideo(event.stream);
    }


    queue.onmessage = function (event) {
      console.log("queue signal message");
      console.log(event.data);
      var SDP = JSON.parse(event.data);

      console.log("SDP type=", SDP.messageType || SDP.errorType);

      pc.processSignalingMessage((env.browser ? "SDP\n" + event.data : "" + JSON.stringify(SDP)));
    }

    if (localStream) {
      pc.addStream(localStream);
    } else {
      pc.addStream();
    }

    pc.onaddstream = function (event) {
      !!UI && UI.showRemoteVideo(event.stream);
    }
}

/**
  * Answer a "call"
  */
function answer(event) {
    pc = new Peer(function (message) {
      var signal = JSON.parse(env.browser ? message.substr(3) : message);
      queue.send(JSON.stringify(signal));
    });

    queue.onmessage = function (event) {
      console.log("signal message");
      pc.processSignalingMessage( (env.browser ? "SDP\n" : "") + event.data);
    }

    if (event) {
      console.log("process signaling message");
      pc.processSignalingMessage( (env.browser ? "SDP\n" : "") + event.data);
    }

    if (env.browser) {
      navigator.webkitGetUserMedia(captureOptions, function (stream) {

        !!UI && UI.showLocalVideo(stream);
        pc.addStream(stream);
      }, onCaptureError);
    } else if (env.node) {
      pc.addStream();
    }
}

var captureOptions = env.capture;

/**
  * getUserMedia success callback
  */
function onCaptureSuccess(stream) {
    call(stream);
}

/**
  * getUserMedia error callback
  */
function onCaptureError(err) {
    UI.show(UI.errorLabel);
}

/**
  * Connect and answer on call
  */
queue.onmessage = function (event) {
  env.caller = false;
  
  console.log("answering");
  answer(event);
  
}

console.log("queue connecting");
queue.connect();

// Node.JS
if (env.node && env.caller) {
  call();
  console.log("calling");
}

// Browser
if (env.browser && env.caller) {
  navigator.webkitGetUserMedia(captureOptions, onCaptureSuccess, onCaptureError);
}

