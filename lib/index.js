var path = require('path');
var binary = require('node-pre-gyp');
var path = require('path');
var webrtc = require(binary.find(path.resolve(path.join(__dirname,'../package.json'))));
module.exports = webrtc;
