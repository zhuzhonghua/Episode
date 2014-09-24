var myutil      = require('./myutil');

var dnode       = require('dnode');

var net = require('net');

var LOG         = myutil.LOG;
var CRASH       = myutil.CRASH;
var cbFunc      = myutil.cbFunc;
var ERROR       = myutil.ERROR;

var RPC = module.exports = {};

RPC.rpcServer = null;
RPC.rpcServerOption = {};

RPC.initServer = function(port, handler){
    var server = net.createServer(function (c) {
        var d = RPC.rpcServer = dnode(handler);
        c.on('error', cbFunc(function(){
            ERROR('[rpc] error ');
        }));
        LOG("[rpc] new client");
        c.pipe(d).pipe(c);
    });

    server.listen(port);
    LOG("[rpc] listen on "+port);
};


RPC.rpcClient = null;
RPC.rpcClientOption = {};

RPC.initClient = function(pport, phost){
    RPC.rpcClientOption = {port: pport,
                            host: phost};
    
    var d = dnode.connect(pport, phost);
    LOG("[rpc] connecting "+pport+" "+phost);
    d.on('remote', function(remote){
	RPC.rpcClient= remote;
	LOG("[rpc] game rpc remote init");
    });

    d.on('error', function(){
        ERROR("[rpc] error "+RPC.rpcClientOption.port+" "+RPC.rpcClientOption.host);
    	remoteError();
    });
    
    d.on('end', function(){
        ERROR("[rpc] end "+RPC.rpcClientOption.port+" "+RPC.rpcClientOption.host);
        remoteError();
    });
};

function remoteError(){
    delete RPC.rpcClient;
    RPC.rpcClient = null;

    var remoteErrorTimeout = 5000;
    
    setTimeout(cbFunc(function(){
        RPC.initClient(RPC.rpcClientOption.port, RPC.rpcClientOption.host);        
    }), remoteErrorTimeout);
};


