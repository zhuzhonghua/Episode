var fs 		= require('fs');
var http 	= require('http');
var https 	= require('https');
var url 	= require('url');
var querystring = require('querystring');
var util 		= require('util');

var LOGS = new Array();
var ERRORS = new Array();

var CRASHS = new Array();

var filePre = "log";

function LOG(cxt)
{
	var d = new Date();
	var cnt = "("+d.getHours()+":"+d.getMinutes()+":"+d.getSeconds()+" "+d.getMilliseconds()+") "+" "+cxt+"\r\n";
	LOGS.push(cnt);
	console.log(cnt);
	
	var filename = filePre+'-'+(d.getMonth()+1)+'-'+d.getDate()+'.log';
	if(LOGS.length <= 1){
		writeLOG(filename, LOGS);
	}
}

function writeLOG(filename, arr)
{
	if(arr.length > 0){
		var cnt = arr[0];
		fs.appendFile(filename, cnt, function(err){
			if(err) throw err;
			arr.shift();
			setImmediate(function(){
				writeLOG(filename, arr);
			});
		});
	}
}

function ERROR(cxt)
{
	var d = new Date();
	var cnt = "("+d.getHours()+":"+d.getMinutes()+":"+d.getSeconds()+" "+d.getMilliseconds()+") [ERROR]"+cxt+"\r\n";
	ERRORS.push(cnt);
	console.error(cnt);
	
	var filename = 'error-'+filePre+'-'+(d.getMonth()+1)+'-'+d.getDate()+'.log';
	if(ERRORS.length <= 1){
		writeLOG(filename, ERRORS);
	}
}

function CRASH(cxt)
{
	var d = new Date();
	var cnt = "("+d.getHours()+":"+d.getMinutes()+":"+d.getSeconds()+" "+d.getMilliseconds()+") [CRASH]"+cxt+"\r\n";
	CRASHS.push(cnt);
	console.log(cnt);
	
	var filename = 'crash-'+filePre+'-'+(d.getMonth()+1)+'-'+d.getDate()+'.log';
	if(CRASHS.length <= 1){
		writeLOG(filename, CRASHS);
	}
}

function DEBUG_BYTE(buf)
{
	LOG(util.inspect(buf));
}

function cbFunc(cb){
	return function(){
		try{
			cb.apply(cb, arguments)
		}
		catch(e){
			CRASH(e.stack);
		}
	}	
}

exports.LOG 	= LOG;
exports.ERROR 	= ERROR;
exports.CRASH	= CRASH;
exports.DEBUG_BYTE = DEBUG_BYTE;
exports.cbFunc	   = cbFunc;
