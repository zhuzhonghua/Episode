var net 		= require('net');
var myutil 		= require('./myutil');

var LOG 		= myutil.LOG;

function Acceptor(port, cb)
{
    this.port = port;
    this.server = null;
    this.newSockCB = cb;

    this.server = net.createServer(this.newSockCB);
    this.server.listen(this.port);
    LOG("acceptor init on port="+this.port);
}

Acceptor.prototype.init = function(){
}

module.exports = Acceptor;
