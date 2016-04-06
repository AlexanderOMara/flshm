// Output field.
createTextField(
	'txtout',
	this.getNextHighestDepth(),
	10,
	10,
	600 - 20,
	400 - 20
);
txtout.multiline = true;
txtout.border = true;

var lc = new LocalConnection();
var n = ['connect', 'selfsending'];
n = n.join('-');
lc.output = function(str) {
	txtout.text = str + '\n' + txtout.text;
};
lc.connect(n);
setInterval(function() {
	var d = new Date(); 
	lc.send(n, 'output', 'time:' + d.getTime());
}, 1000);
