var lc = new LocalConnection();
setInterval(function() {
	var d = new Date();
	lc.send('connection-strnullbyte', 'str' + String.fromCharCode(0) + 'byte', 'time:' + d.getTime());
}, 1000);
