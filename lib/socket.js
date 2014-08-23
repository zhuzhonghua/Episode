var net         = require('net');

var ByteBuffer  = require('./base_types').ByteBuffer;
var DEFINE      = require('./base_define');

var myutil 		= require('./myutil');
var LOG 		= myutil.LOG;
var ERROR 		= myutil.ERROR;
var CRASH		= myutil.CRASH;
var DEBUG_BYTE  = myutil.DEBUG_BYTE;

function Socket(option)
{
    this.option     = option;
    
    this.inData 	= new Array();
    this.outData 	= new Array();
    this.writeBuffer    = new ByteBuffer(1024);
    
    this.active         = false;
    
    this.container      = null;

    var self = this;
    // if client, connect to a address
    if(option.client){
        this._sock = net.Socket();        
        this._sock.connect(option.port, option.host, function(){
            LOG("connected to "+option.port+" "+option.host);
            self.active = true;
            option.connSuccCB(self);
        });
        this._sockID = option.host+'('+option.port+')';
    }
    else{
        this._sock = option.sock;
        this.active = true;
            
        this._sockID = this._sock.remoteAddress+'('+this._sock.remotePort+')';
    }
    
    this.msgCB = option.msgCB;
    this.errCB = option.errCB;
    
    // TODO: remove from this file
    // for game-server's gameclient
    this.account = "";
    // end TODO

    this.init();

    LOG("new socket "+this._sockID);
}

Socket.prototype.close  = function(){
    this.active     = false;
    this.inData     = null;
    this.outData    = null;
    if(this._sock){
        this._sock.destroy();
    }    
    this._sock      = null;
    
    this._sockID    = '';
    
    this.account    = '';
};

Socket.prototype.id = function(){
    return this._sockID;
};

Socket.prototype.sockErr = function(){
    ERROR("sockErr "+this.id());
    this.active = false;
    var self = this;
    this.errCB(self);
    self.close();
};

Socket.prototype.connect = function(ip, port){
    var self = this;
    if(this.active == true){
    	ERROR("already connected "+this.option.host+" "+this.option.port);
    }
};

Socket.prototype.init = function(){
    // data is a Buffer, not a string
    var self = this;
    self._sock.on('data', function (data){
    	LOG(self._sockID + ' data recv ' + data.length);
    	DEBUG_BYTE(data);
    	self.inData.push(data);

    	self.handleInData();
    });
    
    self._sock.on('close', function (data){
    	LOG(self._sockID + ' sock end');
    	//removeSockObj(sockObj);
        if(self._sock){
            self.sockErr();
        }
    });
    
    self._sock.on('error', function (data){
    	LOG(self._sockID + ' sock error');
        if(self._sock){
            self.sockErr();
        }
    });
};


// handle msg
Socket.prototype.handleInData = function()
{
    var self = this;
    var sockObj = self;

    if(self.active == false){
        ERROR("self active false, enter handleInData");
        return;
    }
    
try{
    //var inBufArray = self.inData;
    if(false == self.checkLenData(DEFINE.MSG_HEADER_SIZE)){
    	return;
    }
    var header = self.peekLenData(DEFINE.MSG_HEADER_SIZE);
    var msgID = header.readUInt16();
    var msgSize = header.readUInt32();
    
    if(false == self.checkLenData(msgSize+DEFINE.MSG_HEADER_SIZE)){
    	LOG(sockObj._sockID + ' not enough buffer msgsize='+msgSize+', id='+msgID);	
    	return;
    }
    
    LOG(sockObj._sockID + ' get msg ' + msgID + ' size ' + msgSize);

    self.skipLenData(DEFINE.MSG_HEADER_SIZE);
    var msgBody = self.readLenData(msgSize);

    if(false == self.msgCB(self, msgID, msgBody.buffer)){
        ERROR('false!!! handle msg ' + msgID);
    }
    
    setImmediate(function(){
    	self.handleInData();
    });
}
catch(e){    
    self.sockErr();
    CRASH(e.stack);    
}
};


Socket.prototype.checkLenData = function (len)
{
    var bufArray = this.inData;
    for(var i = 0; i < bufArray.length; ++i){
    	var buf = bufArray[i];
    	// ok
    	if(buf.length >= len){
    	    return true;
    	}
    	// len -= buf.length
    	else{
    	    len -= buf.length;
    	}
    }
    
    return false;
};

// read and remove data from buffer array
Socket.prototype.readLenData = function(len)
{
    var bufArray = this.inData;
    var ret = new Buffer(len);
    var offset = 0;
    while(bufArray.length > 0){
    	var buf = bufArray[0];
    	if(buf.length == len){
 	        buf.copy(ret, offset, 0, buf.length);
    	    bufArray.shift();
    	    
    	    return new ByteBuffer(ret);
    	}
    	else if(buf.length > len){
    	    buf.copy(ret, offset, 0, len);
    	    // slice source data
    	    bufArray[0] = buf.slice(len);
    	    
    	    return new ByteBuffer(ret);
    	}
    	else{
    	    buf.copy(ret, offset, 0, buf.length);
    	    offset += buf.length;
    	    len -= buf.length;
    	    bufArray.shift();
    	}
    }
    return new ByteBuffer(ret);
};

// do not remove data from buffer array
Socket.prototype.peekLenData = function(len)
{
    var bufArray = this.inData;
    var ret = new Buffer(len);
    var offset = 0;
    for(var i = 0; i < bufArray.length; ++i){
    	var buf = bufArray[i];
    	if(buf.length == len){
    	    buf.copy(ret, offset, 0, buf.length);
    	    
    	    return new ByteBuffer(ret);
    	}
    	else if(buf.length > len){
    	    buf.copy(ret, offset, 0, len);
    	    
    	    return new ByteBuffer(ret);
    	}
    	else{
    	    buf.copy(ret, offset, 0, buf.length);
    	    offset += buf.length;
    	    len -= buf.length;
    	}
    }
    return new ByteBuffer(ret);
};

// read and remove data from buffer array
Socket.prototype.skipLenData = function(len)
{
    var bufArray = this.inData;
    while(bufArray.length > 0){
    	var buf = bufArray[0];
    	if(buf.length == len){
  	    bufArray.shift();
    	    return;
    	}
    	else if(buf.length > len){
    	    // slice source data
    	    bufArray[0] = buf.slice(len);
    	    return;
    	}
    	else{
    	    len -= buf.length;
    	    bufArray.shift();
    	}
    }
};

Socket.prototype.sendByteBuffer = function(opcode, bufMsg){
    if(this.active == false){
    	return;
    }

    var body = new ByteBuffer(bufMsg);
    
    // head
    var head = new ByteBuffer(DEFINE.MSG_HEADER_SIZE);
    head.writeUInt16(opcode);
    var pack = (DEFINE.PACKET_HEAD << 24 >>> 0) + body.buffer.length;
    //LOG("pack "+body.buffer.length);
    head.writeUInt32(pack);
    this.outData.push(head.buffer);
    
    // body
    this.outData.push(body.buffer);
    LOG(this._sockID + ': send msg ' + opcode + ' size ' + body.buffer.length);
    DEBUG_BYTE(head.buffer);
    DEBUG_BYTE(body.buffer);
    
    this.flushOutData();
};

Socket.prototype.flushOutData = function(){
    var sockObj = this;
    var outBuf = sockObj.outData;
    if(outBuf && outBuf.length > 0){
    	var buf = outBuf[0];
    	var ret = this.writeData(buf);
    
    	// send all ok
    	if(true == ret){
    	    LOG(sockObj._sockID + ': flush data full ' + buf.length);
    	    outBuf.shift();
    	    setImmediate(function(){
    	    	sockObj.flushOutData();
    	    });
    	}
    	// wait
    	else if(false == ret){
    	    setImmediate(function(){
    	    	sockObj.flushOutData();
    	    });
    	}
    	// send partial
    	else{
    	    LOG(sockObj._sockID + ': flush data partial ' + ret);
    	    outBuf[0] = buf.slice(ret);
    	    setImmediate(function(){
    	    	sockObj.flushOutData();
    	    });
    	}
    }
};

// check if there any data pending
Socket.prototype.writeData = function(data){
    var sock = this._sock;
    if(sock.bufferSize > 0){
    	return false;
    }
    else{
    	// return ok
    	if(true == sock.write(data)){
    	    return true;
    	}
    	// or queue buffer size
    	else{
    	    return sock.bufferSize;
    	}
    }
};

module.exports = Socket;
