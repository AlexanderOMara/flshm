WIDTH = 600;
HEIGHT = 400;

// Output field.
createTextField(
	'txtout',
	this.getNextHighestDepth(),
	10,
	10,
	WIDTH - 20,
	HEIGHT / 2 - 40
);
txtout.multiline = true;
txtout.border = true;

// Input field.
createTextField(
	'txtin',
	this.getNextHighestDepth(),
	10,
	HEIGHT / 2 - 20,
	WIDTH - 20,
	HEIGHT / 2 - 20
);
txtin.multiline = true;
txtin.border = true;
txtin.type = 'input';

// Status field.
createTextField(
	'txtstat',
	this.getNextHighestDepth(),
	10,
	HEIGHT - 30,
	WIDTH / 2,
	20
);
txtstat.multiline = true;
txtstat.border = true;

// Create button.
this.createEmptyMovieClip('btnsend', this.getNextHighestDepth());
btnsend._x = WIDTH - 90;
btnsend._y = HEIGHT - 30;

// Add background.
btnsend.createEmptyMovieClip('btnsendbg', btnsend.getNextHighestDepth());
btnsend.btnsendbg.lineStyle(0, 0x000000, 60, true, 'none', 'square', 'round');
btnsend.btnsendbg.beginFill(0xDDDDDD);
btnsend.btnsendbg.lineTo(80, 0);
btnsend.btnsendbg.lineTo(80, 20);
btnsend.btnsendbg.lineTo(0, 20);
btnsend.btnsendbg.lineTo(0, 0);
btnsend.btnsendbg.endFill();

// Add text.
btnsend.createTextField(
	'btntxt',
	btnsend.getNextHighestDepth(),
	0,
	2,
	80,
	16
);
btnsend.btntxt.selectable = false;
var sendText = 'Send';
btnsend.btntxt.text = sendText;

// Create local connection and connect to the peer.
var lc = new LocalConnection();
lc.connect('connection-' + MESSANGER_SELF);
lc.onStatus = function(infoObject) {
	// Apparently the only property, either 'status' or 'error'.
	txtstat.text = 'infoObject.level: ' + infoObject.level;
};
lc.output = function(str) {
	txtout.text = str;
};
btnsend.onRelease = function() {
	txtstat.text = 'pending';
	var ret = lc.send('connection-' + MESSANGER_PEER, 'output', txtin.text);
	btnsend.btntxt.text = sendText + ': ' + ret;
};
