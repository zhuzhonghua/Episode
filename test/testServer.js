var myutil      = require('../lib/myutil');
var Acceptor 	= require('../lib/acceptor');
var Socket      = require('../lib/socket');
var RPC         = require('../lib/rpc');

var LOG 	= myutil.LOG;
var ERROR   = myutil.ERROR;
var cbFunc 	= myutil.cbFunc;

var op = require('./op');

var allClient = {}

function msgCB(session, msgID, buf)
{
	if(msgID == 1){
		var inst = op.testMessage.decode(buf);
		LOG("recv "+msgID+" "+inst.hello);

		var msg = new op.testMessage({hello:"world"});
		session.sendByteBuffer(2, msg.toBuffer());
	}
}

function errCB(session)
{
	delete allClient[session.id()];
}


function newSock(newSock)
{
	var options = {	sock:newSock, 
					errCB:cbFunc(errCB),
					msgCB:cbFunc(msgCB)};

	var session = new Socket(options);
	allClient[session.id()] = session;

	LOG("new client "+session.id());
}

var acceptorClient = new Acceptor(1234, newSock);

var rpcHandler = {test: function(s, cb){
	cb(s+"2");
}};

RPC.initServer(9527, rpcHandler);
