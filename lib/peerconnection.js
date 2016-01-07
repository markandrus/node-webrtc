var path = require('path');
var binary = require('node-pre-gyp');
var path = require('path');
//var binding_opts = require(path.resolve(path.join(__dirname, '../build/wrtc.json')));
var binding_path = binary.find(path.resolve(path.join(__dirname,'../package.json')));
var _webrtc = require(binding_path);

var EventTarget = require('./eventtarget');

//var MediaStream               = require('./mediastream');
//var MediaStreamEvent          = require('./mediastreamevent');
var RTCDataChannel            = require('./datachannel');
var RTCDataChannelEvent       = require('./datachannelevent');
var RTCError                  = require('./error');
var RTCIceCandidate           = require('./icecandidate');
var RTCPeerConnectionIceEvent = require('./rtcpeerconnectioniceevent');
var RTCSessionDescription     = _webrtc.RTCSessionDescription;
var RTCStatsResponse          = require('./rtcstatsresponse');

function RTCPeerConnection(configuration, constraints) {
  'use strict';
  var that = this
    , pc = new _webrtc.PeerConnection(configuration, constraints)
    , localType = null
    , remoteType = null
    , queue = []
    , pending = null
    , dataChannels = {};  // open data channels, indexed by label

  EventTarget.call(this);

  function checkClosed() {
//    if(this._closed) {
//      throw new Error('Peer is closed');
//    }
  }

  function executeNext() {
    if(queue.length > 0) {
      var obj = queue.shift();

      pc[obj.func].apply(pc, obj.args);

      if(obj.wait)
      {
        pending = obj;
      } else {
        executeNext();
      }
    } else {
      pending = null;
    }
  }

  function queueOrRun(obj) {
    checkClosed();

    if(null === pending) {
      pc[obj.func].apply(pc, obj.args);

      if(obj.wait) {
        pending = obj;
      }
    } else {
      queue.push(obj);
    }
  }

  function runImmediately(obj) {
    checkClosed();

    return pc[obj.func].apply(pc, obj.args);
  }

  //
  // Attach events to the native PeerConnection object
  //

  pc.onerror = function onerror() {
    if(pending && pending.onError) {
      pending.onError.apply(that, arguments);
    }

    executeNext();
  };

  pc.onsuccess = function onsuccess() {
    if(pending && pending.onSuccess) {
      pending.onSuccess.apply(that, arguments);
    }

    executeNext();
  };

  pc.onicecandidate = function onicecandidate(candidate, sdpMid, sdpMLineIndex) {
    var icecandidate = new RTCIceCandidate({
      candidate:     candidate,
      sdpMid:        sdpMid,
      sdpMLineIndex: sdpMLineIndex
    });

    that.dispatchEvent(new RTCPeerConnectionIceEvent('icecandidate', {candidate: icecandidate}));
  };

  pc.onsignalingstatechange = function onsignalingstatechange(state) {
    var stateString = that.RTCSignalingStates[state];

    if('closed' === stateString) {
      Object.keys(dataChannels).forEach(function(label) {
        dataChannels[label].shutdown();

        delete dataChannels[label];
      });
    }

    that.dispatchEvent({type: 'signalingstatechange'});
  };

  pc.oniceconnectionstatechange = function oniceconnectionstatechange(state) {
    that.dispatchEvent({type: 'iceconnectionstatechange'});
  };

  pc.onicegatheringstatechange = function onicegatheringstatechange(state) {
    that.dispatchEvent({type: 'icegatheringstatechange'});

    // if we have completed gathering candidates, trigger a null candidate event
    if (that.RTCIceGatheringStates[state] === 'complete') {
      that.dispatchEvent(new RTCPeerConnectionIceEvent('icecandidate', {candidate: null}));
    }
  };

  // [ToDo] onnegotiationneeded

  pc.ondatachannel = function ondatachannel(internalDC) {
    dataChannels[internalDC.label] = internalDC;

    var dc = new RTCDataChannel(internalDC);

    that.dispatchEvent(new RTCDataChannelEvent('datachannel', {channel: dc}));
  };

  pc.onaddstream = function onaddstream(internalMS) {
    var ms = new MediaStream(internalMS);

    that.dispatchEvent(new MediaStreamEvent('addstream', {stream: ms}));
  };

  pc.onremovestream = function onremovestream(internalMS) {
    var ms = new MediaStream(internalMS);

    that.dispatchEvent(new MediaStreamEvent('removestream', {stream: ms}));
  };

  //
  // PeerConnection properties & attributes
  //

  Object.defineProperties(this, {
    'localDescription': {
      get: function() {
        console.log(pc);
        console.log(pc.localDescription);
        return pc.localDescription;
      }
    },
    'remoteDescription': {
      get: function() {
        return pc.remoteDescription;
      }
    },
    'signalingState': {
      get: function getSignalingState() {
        var state = pc.signalingState;
        return this.RTCSignalingStates[state];
      }
    },
    'readyState': {
      get: function getReadyState() {
        return pc.getReadyState();
      }
    },
    'iceGatheringState': {
      get: function getIceGatheringState() {
        var state = pc.iceGatheringState;
        return this.RTCIceGatheringStates[state];
      }
    },
    'iceConnectionState': {
      get: function getIceConnectionState() {
        var state = pc.iceConnectionState;
        return this.RTCIceConnectionStates[state];
      }
    }
  });

  //
  // PeerConnection methods
  //

  this.createOffer = pc.createOffer.bind(pc);

  this.createAnswer = pc.createAnswer.bind(pc);

  this.setLocalDescription = pc.setLocalDescription.bind(pc);

  this.setRemoteDescription = pc.setRemoteDescription.bind(pc);

  this.addIceCandidate = pc.addIceCandidate.bind(pc);

  this.createDataChannel = pc.createDataChannel.bind(pc);

  this.addStream = function addStream(stream, constraintsDict) {
    constraintsDict = constraintsDict || {};

    queueOrRun({
      func: 'addStream',
      args: [stream._getMS(), constraintsDict]
    });
  };

  this.removeStream = function removeStream(stream) {
    queueOrRun({
      func: 'removeStream',
      args: [stream._getMS()]
    });
  };

  this.getLocalStreams = function getLocalStreams() {
    return []; // pc.getLocalStreams();
  };

  this.getRemoteStreams = function getRemoteStreams() {
    return []; // pc.getRemoteStreams();
  };

  this.getStats = function getStats(onSuccess, onFailure) {
    pc.getStats(function(internalRTCStatsResponse) {
      onSuccess(new RTCStatsResponse(internalRTCStatsResponse));
    }, onFailure);
  };

  this.getStreamById = function getStreamById(streamId) {
    return pc.getStreamById(streamId);
  };

  this.close = function close() {
    return runImmediately({
      func: 'close',
      args: []
    });
  };
}

RTCPeerConnection.prototype.RTCIceConnectionStates = [
  'new',
  'checking',
  'connected',
  'completed',
  'failed',
  'disconnected',
  'closed'
];

RTCPeerConnection.prototype.RTCIceGatheringStates = [
  'new',
  'gathering',
  'complete'
];

RTCPeerConnection.prototype.RTCSignalingStates = [
  'stable',
  'have-local-offer',
  'have-local-pranswer',
  'have-remote-offer',
  'have-remote-pranswer',
  'closed'
];


module.exports = RTCPeerConnection;
