var UINT32MAX = 4294967296;
var Int64 = require('./Int64');

function ByteBuffer(para)
{
	this.pos = 0;
	// read
	if(Buffer.isBuffer(para)){
		this.buffer = para;
	}
	// write, len
	else{
		this.buffer = new Buffer(para);
	}
	
	this.toString = function(){
		return this.buffer.toString('hex', this.pos);
	};
	this.readInt8 = function(){
		var value = this.buffer.readInt8(this.pos);
		this.pos += 1;
		return value;
	}
	this.readUInt8 = function(){
		var value = this.buffer.readUInt8(this.pos);
		this.pos += 1;
		return value;
	}
	this.readInt16 = function(){
		var value = this.buffer.readInt16LE(this.pos);
		this.pos += 2;
		return value;
	}
	this.readUInt16 = function(){
		var value = this.buffer.readUInt16LE(this.pos);
		this.pos += 2;
		return value;
	}
	this.readInt32 = function(){
		var value = this.buffer.readInt32LE(this.pos);
		this.pos += 4;
		return value;
	}
	this.readUInt32 = function(){
		var value = this.buffer.readUInt32LE(this.pos);
		this.pos += 4;
		return value;
	}
	this.readFloat = function(){
		var value = this.buffer.readFloatLE(this.pos);
		this.pos += 4;
		return value;
	}	
	this.readInt64 = function(){
		var lo = this.buffer.readInt32LE(this.pos);
		this.pos += 4;
		var hi = this.buffer.readInt32LE(this.pos);
		this.pos += 4;
		var x = new Int64(hi, lo);
		return x.toNumber(true);
	}
	this.readUInt64 = function(){
		var value = this.buffer.readUInt32LE(this.pos);
		this.pos += 4;
		var valueHigh = this.buffer.readUInt32LE(this.pos);
		this.pos += 4;
		return valueHigh*UINT32MAX+value;
	}
	this.readStr = function(len){
		if(len <= 0){
			return '';
		}
		else if(this.buffer.length - this.pos >= len){
			var value = this.buffer.toString('utf8', this.pos, this.pos+len);
			this.pos += len;
			return value;
		}
		else{
			return null;
		}
	}

	///////////////////
	this.writeInt8 = function(value){
		this.buffer.writeInt8(value, this.pos);
		this.pos += 1;
	}
	this.writeUInt8 = function(value){
		this.buffer.writeUInt8(value, this.pos);
		this.pos += 1;
	}
	this.writeInt16 = function(value){
		this.buffer.writeInt16LE(value, this.pos);
		this.pos += 2;
	}
	this.writeUInt16 = function(value){
		this.buffer.writeUInt16LE(value, this.pos);
		this.pos += 2;
	}
	this.writeInt32 = function(value){
		this.buffer.writeInt32LE(value, this.pos);
		this.pos += 4;
	}
	this.writeUInt32 = function(value){
		this.buffer.writeUInt32LE(value, this.pos);
		this.pos += 4;
	}
	this.writeFloat = function(value){
		this.buffer.writeFloatLE(value, this.pos);
		this.pos += 4;
	}
	this.writeInt64 = function(value){
		var lo = value & 0xffffffff;
		var hi = value / UINT32MAX >> 0;
		this.buffer.writeInt32LE(lo, this.pos);
		this.pos += 4;
		this.buffer.writeInt32LE(hi, this.pos);
		this.pos += 4;
	}
	this.writeUInt64 = function(value){
		this.buffer.writeUInt32LE(value&0xffffffff, this.pos);
		this.pos += 4;
		value = value >>> 31;
		value = value >>> 1;
		this.buffer.writeUInt32LE(value, this.pos);
		this.pos += 4;
	}
	this.writeStr = function(value){
		if(value.length <= 0){
			return;
		}
		this.buffer.write(value, this.pos, value.length);
		this.pos += value.length;
	}
}

exports.ByteBuffer = ByteBuffer;
