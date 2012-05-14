require('console-trace');

console.log("node " + process.version);

var express = require('express')
  , argv = require('optimist')
	.usage("Usage: signaller.js --port [num]")
		.demand(['port'])
		.argv

  , WebSocketServer = require('ws').Server
  , WebSocket = require('ws');


var app = express.createServer()  
  , queue = new WebSocketServer({ 
		server: app
	})
  , channel = {
  	  src: null
  	, src_queue: []

    , dest: null
    , dest_queue: []
  };

queue.on('connection', function (connection) {
	var isCallee = !channel.dest
	  , isCaller = !channel.src
	  , ch;

	connection.on('message', function (message) {
		ch = channel[isCaller ? 'dest' : 'src'];

		if (ch == null) {
			channel[isCaller ? 'dest_queue' : 'src_queue'].push(message.data);
			return;
		}
		
		console.t.log("channel available signaling");

		ch.send(message);
	});

	connection.on('close', function (message) {
		console.t.error((isCaller ? 'source' : 'destination') + " closing");
		
		channel[isCaller ? 'src' : 'dest'] = null;
		channel = {
		   src: null
	  	 , src_queue: []
	     , dest: null
	     , dest_queue: []
		};
	});

	if (isCaller) {
		channel.src = connection;
		console.t.log("source opening");

	} else if (isCallee) {
		channel.dest = connection;
		console.t.log("destination opening");
	}
});

console.t.log("Creating queue");

app.listen(argv.port);