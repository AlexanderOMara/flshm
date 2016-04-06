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

// Create local connection and connect to the peer.
var lc = new LocalConnection();
lc.connect('connection-universal');
lc.output = function(str) {
	txtout.text = str + '\n' + txtout.text;
};
