var lc = new LocalConnection();
var n = ['connect', 'multipleargs'];
n = n.join('-');
setInterval(function() {
	var d = new Date(); 
	lc.send(n, 'output', true, 'two', d.getTime());
}, 1000);
