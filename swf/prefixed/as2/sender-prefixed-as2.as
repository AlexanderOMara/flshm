var lc = new LocalConnection();
setInterval(function() {
	var d = new Date();
	lc.send('_connection-universal', 'output', 'time:' + d.getTime());
}, 1000);
