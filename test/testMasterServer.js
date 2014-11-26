var myutil      = require('../lib/myutil');
var Acceptor 	= require('../lib/acceptor');
var Socket      = require('../lib/socket');
var http        = require('http');

var LOG 	= myutil.LOG;
var ERROR   = myutil.ERROR;
var cbFunc 	= myutil.cbFunc;

var op = require('./op');

var rpcMsgIndex = 0;
var rpcMsgIndexHandler = {};

function msgCB(session, msgID, buf)
{
	if(msgID == 2){
		var inst = op.testRPCMessage_RET.decode(buf);
		LOG("recv "+inst.index);

        rpcMsgIndexHandler[inst.index](session, msgID, buf);
	}
}

function testSendRPCMsg(cb){
    for(var i in allChild){
        // this tempIndex is important
        // it's index for the callback function clozure
        var tempIndex = ++rpcMsgIndex;

        var toHand = setTimeout(cbFunc(function(){
            // for timeout not response
            delete rpcMsgIndexHandler[tempIndex];
        }), 10*1000);
        
        rpcMsgIndexHandler[tempIndex] = function(session, msgID, buf){
            // clear the timeout handler
            clearTimeout(toHand);
            // remove this msg index
            delete rpcMsgIndexHandler[tempIndex];
            LOG("handled====="+tempIndex);
            cb();
        }

        // send to every child for test
        var inst = new op.testRPCMessage({index:tempIndex});
        allChild[i].sendByteBuffer(1, inst.toBuffer());
    }
}

var allChild = {}
function errCB(session)
{
	delete allChild[session.id()];
}
function newSock(newSock)
{
	var options = {	sock:newSock, 
					errCB:cbFunc(errCB),
					msgCB:cbFunc(msgCB)};

	var session = new Socket(options);
	allChild[session.id()] = session;

	LOG("new child server "+session.id());
}

var acceptorClient = new Acceptor(1234, newSock);

//===================
// just for trigger push msg to child server
function handleRequest(req, res){
    testSendRPCMsg(cbFunc(function(){
        res.writeHead(200, {'Content-Type': 'text/plain'});
        res.end("OK");
        res.end('\n');
    }));
}

var server = http.createServer(cbFunc(handleRequest));
server.listen(9527);
