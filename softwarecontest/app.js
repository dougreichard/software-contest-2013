var net = require ('net')
var t = 0;

var client = new net.Socket();
client.on('data', 	function (data) {
	
	console.log('Data: ' + data);
	t++;
	
	//if (t < 17) client.write('{"command":"U"}');
	if (t < 17) client.write('{"host":"card"}');
	else client.destroy();
	
	
});

client.connect(8124, '127.0.0.1', function () {


	var str = '{"host":"card"}';
	var buf = new Buffer(str.length);

	for (var i = 0; i < str.length ; i++) {
		buf[i] = str.charCodeAt(i);
	}
	
	console.log(buf.length);
	
	
	
	/// connected
	client.write(buf);

});
	
