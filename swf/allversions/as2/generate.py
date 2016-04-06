asc = '''
var lc = new LocalConnection();
var a = ['connect', 'swf%i', 'as2'];
lc.connect(a.join('-'));
setInterval(function() {
	var d = new Date();
	lc.send('connection-universal', 'output', 'time:' + d.getTime());
}, 1000);
'''.lstrip()
asf = 'connect-swf%i-as2.as'

mkc = '''\t$(MAKESWF) -v %i \\
\t\t-o $(CURDIR)/allversions/as2/connect-swf%i-as2.swf \\
\t\t$(CURDIR)/allversions/as2/connect-swf%i-as2.as
'''.rstrip()

sef = '\t$(EDITSWFV) %i $(CURDIR)/allversions/as2/connect-swf%i-as2.swf'

# CC 2014 goes up to 28, but testing higher than recognized wouldn't hurt.
for i in range(5, 41):
	if i > 9:
		print(mkc % (9, i, i))
		print(sef % (i, i))
	else:
		print(mkc % (i, i, i))
	with open(asf % (i), 'w') as f:
		f.write(asc % (i))
		f.close()
