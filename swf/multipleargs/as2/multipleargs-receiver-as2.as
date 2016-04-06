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
var n = ['connect', 'multipleargs'];
n = n.join('-');
lc.output = function(a, b, c) {
	txtout.text = a + ', ' + b + ', ' + c + '\n' + txtout.text;
};
lc.connect(n);
