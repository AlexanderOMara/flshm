var lc = new LocalConnection();
var a = ['connect', 'swf35', 'as2'];
lc.connect(a.join('-'));
setInterval(function() {
	var d = new Date();
	lc.send('connection-universal', 'output', 'time:' + d.getTime());
}, 1000);
