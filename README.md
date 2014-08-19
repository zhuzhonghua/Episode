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

---

使用方法参考test下的testServer.js和testClient.js和testClient.js

启动测试服务器

	node testServer.js
	
启动测试客户端

	node testClient.js