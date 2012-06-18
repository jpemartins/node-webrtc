// Copyright (C) 2012 Google. All rights reserved.
/**
 * @fileoverview
 * An implementation of ROAP that builds on JSEP as a substrate.
 * This is based on the following drafts:<ul>
 * <li>draft-uberti-rtcweb-jsep-02 (JSEP).
 * <li>draft-jennings-rtcweb-signaling-01 (ROAP).
 * </ul>
 *
 * Liberties have been taken with the APIs, such as using strings
 * rather than constants for all state variable values.
 * @author hta@google.com (Harald Alvestrand)
 */
// Static variable for allocating new session IDs.
RoapConnection.sessionId = 103;

// Call the constructor for peerconnections indirectly, so that it's availble
// for dependency injection.
/** Constructor for JSEP supporting PeerConnection objects. */
RoapConnection.JsepPeerConnectionConstructor = webkitPeerConnection00;
/** Constructor for SessionDescription objects. */
RoapConnection.SessionDescriptionConstructor = SessionDescription;

/**
 * @constructor for RoapConnection objects.
 * @param {string} configuration Configuration details. Ignored.
 * @param {function} signalingCallback for processing signalling messages.
 */
function RoapConnection(configuration, signalingCallback) {
  var that = this;
  this.peerConnection = new RoapConnection.JsepPeerConnectionConstructor(
      'STUN stun.l.google.com:19302',
      function(candidate, more) {
        if (more == false) {
          // At the moment, we do not renegotiate when new candidates
          // show up after the more flag has been false once.
          that.moreIceComing = false;
          that.markActionNeeded();
        }
        that.iceCandidateCount += 1;
      });
  this.sessionId = ++RoapConnection.sessionId;
  this.sequenceNumber = 0;  // Number of last ROAP message sent. Starts at 1.
  this.actionNeeded = false;
  this.iceStarted = false;
  this.moreIceComing = true;
  this.iceCandidateCount = 0;
  this.onsignalingmessage = signalingCallback;
  this.peerConnection.onopen = function() {
    if (that.onopen) {
      that.onopen();
    }
  }
  this.peerConnection.onaddstream = function(stream) {
    if (that.onaddstream) {
      that.onaddstream(stream);
    }
  }

  this.peerConnection.onremovestream = function(stream) {
    if (that.onremovestream) {
      that.onremovestream(stream);
    }
  }
  // Variables that are part of the public interface of PeerConnection
  // in the 28 January 2012 version of the webrtc specification.
  this.onaddstream = null;
  this.onremovestream = null;
  this.state = 'new';
  // Auto-fire next events.
  this.markActionNeeded();
}

/**
 * This function processes signalling messages from the other side.
 * @param {string} msgstring JSON-formatted string containing a ROAP message.
 */
RoapConnection.prototype.processSignalingMessage = function(msgstring) {
  // Offer: Check for glare and resolve.
  // Answer/OK: Remove retransmit for the msg this is an answer to.
  // Send back "OK" if this was an Answer.
  console.log('Activity on conn ' + this.sessionId);
  var msg = JSON.parse(msgstring);
  this.incomingMessage = msg;
  if (this.state === 'new') {
    if (msg.messageType === 'OFFER') {
      // Initial offer.
      this.offer_as_string = msg.sdp;
      var sdp = new RoapConnection.SessionDescriptionConstructor(msg.sdp);
      console.log('SDP is ' + sdp.toSdp());
      this.peerConnection.setRemoteDescription(this.peerConnection.SDP_OFFER,
                                               sdp);
      this.state = 'offer-received';
      // Allow other stuff to happen, then reply.
      this.markActionNeeded();
    } else {
      this.error('Illegal message for this state: ' +
                 msg.messageType + ' in state ' + this.state);
    }
  } else if (this.state === 'offer-sent') {
    if (msg.messageType === 'ANSWER') {
      this.peerConnection.setRemoteDescription(this.peerConnection.SDP_ANSWER,
          new RoapConnection.SessionDescriptionConstructor(msg.sdp));
      this.sendOK();
      this.state = 'established';
    } else if (msg.messageType === 'pr-answer') {
      this.peerConnection.setRemoteDescription('pr-answer',
          new RoapConnection.SessionDescriptionConstructor(msg.sdp));
      // No change to state, and no response.
    } else if (msg.messageType === 'offer') {
      // Glare processing.
      this.error('Not written yet');
    } else {
      this.error('Illegal message for this state: ' +
                 msg.messageType + ' in state ' + this.state);
    }
  } else if (this.state === 'established') {
    if (msg.messageType === 'OFFER') {
      // Subsequent offer.
      this.peerConnection.setRemoteDescription(this.peerConnection.SDP_OFFER,
          new RoapConnection.SessionDescriptionConstructor(msg.sdp));
      this.state = 'offer-received';
      // Allow other stuff to happen, then reply.
      this.markActionNeeded();
    } else {
      this.error('Illegal message for this state: ' +
                 msg.messageType + ' in state ' + this.state);
    }
  }
};

/**
 * Adds a stream - this causes signalling to happen, if needed.
 * @param {MediaStream} stream The outgoing MediaStream to add.
 */
RoapConnection.prototype.addStream = function(stream) {
  this.peerConnection.addStream(stream);
  this.markActionNeeded();
};

/**
 * Removes a stream.
 * @param {MediaStream} stream The MediaStream to remove.
 */
RoapConnection.prototype.removeStream = function(stream) {
  var i;
  for (i = 0; i < this.peerConnection.localStreams.length; ++i) {
    if (localStreams[i] === stream) {
      localStreams[i] = null;
    }
  }
  this.markActionNeeded();
};

/**
 * Closes the connection.
 */
RoapConnection.prototype.close = function() {
  this.state = 'closed';
  this.peerConnection.close();
};

/**
 * Internal function: Mark that something happened.
 */
RoapConnection.prototype.markActionNeeded = function() {
  this.actionNeeded = true;
  var that = this;
  this.doLater(function() {
      that.onstablestate();
    });
};

/**
 * Internal function: Do something later (not on this stack).
 * @param {function} what Callback to be executed later.
 */
RoapConnection.prototype.doLater = function(what) {
  // Post an event to myself so that I get called a while later.
  // (needs more JS/DOM info. Just call the processing function on a delay
  // for now.)
  window.setTimeout(what, 1);
};

/**
 * Internal function called when a stable state
 * is entered by the browser (to allow for multiple AddStream calls or
 * other interesting actions).
 * This function will generate an offer or answer, as needed, and send
 * to the remote party using our onsignalingmessage function.
 */
RoapConnection.prototype.onstablestate = function() {
  var mySDP;
  var roapMessage = {};
  if (this.actionNeeded) {
    if (this.state === 'new' || this.state === 'established') {
      // See if the current offer is the same as what we already sent.
      // If not, no change is needed.
      var newOffer;
      try {
        newOffer = this.peerConnection.createOffer('audio,video');
      } catch (e) {
        newOffer = this.peerConnection.createOffer({audio:true, video:true});
      }

      if (newOffer.toSdp() != this.prevOffer) {
        // Prepare to send an offer.
        this.peerConnection.setLocalDescription(this.peerConnection.SDP_OFFER,
                                                newOffer);
        this.peerConnection.startIce();
        this.state = 'preparing-offer';
        this.markActionNeeded();
        return;
      } else {
        console.log('Not sending a new offer');
      }
    } else if (this.state === 'preparing-offer') {
      // Don't do anything until we have the ICE candidates.
      if (this.moreIceComing) {
        return;
      }
      // Now able to send the offer we've already prepared.
      this.prevOffer = this.peerConnection.localDescription.toSdp();
      console.log('Sent SDP is' + this.prevOffer);
      this.sendMessage('OFFER', this.prevOffer);
      // Not done: Retransmission on non-response.
      this.state = 'offer-sent';
    } else if (this.state === 'offer-received') {
      try {
        mySDP = this.peerConnection.createAnswer(this.offer_as_string,
                                                 'audio,video');
      } catch (e) {
        mySDP = this.peerConnection.createAnswer(this.offer_as_string,
                                                 {audio:true, video:true});
      }
      this.peerConnection.setLocalDescription(this.peerConnection.SDP_ANSWER,
                                              mySDP);
      this.state = 'offer-received-preparing-answer';
      if (!this.iceStarted) {
        var now = new Date();
        console.log(now.getTime() + ': Starting ICE in responder');
        this.peerConnection.startIce();
        this.iceStarted = true;
      } else {
        this.markActionNeeded();
        return;
      }
    } else if (this.state === 'offer-received-preparing-answer') {
      if (this.moreIceComing) {
        return;
      }
      mySDP = this.peerConnection.localDescription;
      this.sendMessage('ANSWER', mySDP.toSdp());
      this.state = 'established';
    } else {
      this.error('Dazed and confused in state ' + this.state +
                 ', stopping here');
    }
    this.actionNeeded = false;
  }
};

/**
 * Internal function to send an "OK" message.
 */
RoapConnection.prototype.sendOK = function() {
  this.sendMessage('OK');
};

/**
 * Internal function to send a signalling message.
 * @param {string} operation What operation to signal.
 * @param {string} sdp SDP message body.
 */
RoapConnection.prototype.sendMessage = function(operation, sdp) {
  var roapMessage = {};
  roapMessage.messageType = operation;
  roapMessage.sdp = sdp;  // may be null or undefined
  if (operation === 'OFFER') {
    roapMessage.offererSessionId = this.sessionId;
    roapMessage.answererSessionId = this.otherSessionId;  // may be null
    roapMessage.seq = ++this.sequenceNumber;
    // The tiebreaker needs to be neither 0 nor 429496725.
    roapMessage.tiebreaker = Math.floor(Math.random() * 429496723 + 1);
  } else {
    roapMessage.offererSessionId = this.incomingMessage.offererSessionId;
    roapMessage.answererSessionId = this.sessionId;
    roapMessage.seq = this.incomingMessage.seq;
  }
  this.onsignalingmessage(JSON.stringify(roapMessage));
};

/**
 * Internal something-bad-happened function.
 * @param {string} text What happened - suitable for logging.
 */
RoapConnection.prototype.error = function(text) {
  throw 'Error in RoapOnJsep: ' + text;
};
