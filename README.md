Episode
=======

episode is a gameserver framework based on nodejs

Pomelo追求的是大而全，Episode追求的是小而精。

目前只有网络层的功能。

1. 监听端口
2. 接收数据包
3. 发送数据包
4. 检测连接断开等事件
5. 连接别的网络服务
6. 搭建分布式服务器时的RPC调用

---

使用方法参考test下的testServer.js和testClient.js和testClient.js

启动测试服务器

	node testServer.js
	
启动测试客户端

	node testClient.js
	
---

myutil.LOG(...)

	var myutil = require('../lib/myutil');
	myutil.LOG("init "+new Date());

---

myutil.ERROR(...)	

	同上

---

myutil.CRASH(...) 此方法一般在try catch中使用

	var myutil = require('../lib/myutil');
	try{
		throw Error('error');
	}
	catch(e){
		myutil.CRASH(e.stack);
	}
	
---

myutil.cbFunc(...) 函数回调时如果会出现异常，那么用此方法包装

	var myutil = require('../lib/myutil');
	var fs = require('fs');
	fs.read('abc.txt', myutil.cbFunc(function(content){
		if(content[0] != "1"){ throw Error('error'); }
	}));
	
---

Socket 作为客户端发起tcp连接，或者作为服务器接收tcp连接

	// client	
	var myutil = require('../lib/myutil');
	var Socket = require('../lib/socket');
	var client = new Socket({host: "127.0.0.1",
				 port: 9527,
				 client: true,
				 myutil.cbFunc(function(session, id, buf){
				 	console.log('recv '+id);
				 }),
				 myutil.cbFunc(function(sock){
				 	if(client != sock){
				 		throw Error('internal error');
				 	}
				 })});
				 
	// server				 
	var Acceptor = require('../lib/acceptor');
	var server = new Acceptor(9527, myutil.cbFunc(function(sock){
						var client = new Socket({sock: sock,
									 myutil.cbFunc(function(session, id, buf){
									 	console.log('recv '+id);
									 }),
									 myutil.cbFunc(function(sock){
									 	if(client != sock){
									 		throw Error('internal error');
									 	}
									 })});
					}));
	
---

rpc.initServer 远程过程调用时，开启服务器

	var rpc = require('../lib/rpc');
	rpc.initServer(9527, {AddOne: function(num, cb){
					cb(num+1);
				}});

---

rpc.initClient 远程过程调用时，开始客户端

	var rpc = require('../lib/rpc');
	rpc.initClient(9527, "127.0.0.1");
	setTimeout(function(){
		rpc.rpcClient.AddOne(10, function(num){
						console.log('get num '+num);
					});
	}, 5000);
