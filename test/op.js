var fs        = require('fs');
var Protobuf  = require('protobufjs');

var testBuilder    = Protobuf.loadProtoFile('test.proto');

var test           = testBuilder.build();

var op  = module.exports = {};

op.testMessage = test.testMessage;

