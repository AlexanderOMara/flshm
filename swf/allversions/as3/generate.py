asc = '''
package
{
	import flash.display.MovieClip;
	import flash.net.LocalConnection;
	import flash.utils.setInterval;
	import flash.events.StatusEvent

	public class main extends MovieClip {

		public function main() {
			var lc:LocalConnection = new LocalConnection();
			lc.connect(['connect', 'swf%i', 'as3'].join('-'));
			lc.addEventListener(StatusEvent.STATUS, function(e:*) {
			});
			setInterval(function() {
				var d:Date = new Date();
				lc.send('connection-universal', 'output', 'time:' + d.getTime());
			}, 1000);
		}
	}
}
'''.lstrip()
asf = 'connect-swf%i-as3.as'

mkc = '''$(AS3COMP) -T %i \\
\t-o $(CURDIR)/allversions/as3/connect-swf%i-as3.swf \\
\t$(CURDIR)/allversions/as3/connect-swf%i-as3.as
'''.rstrip()

# CC 2014 goes up to 28, but testing higher than recognized wouldn't hurt.
for i in range(9, 41):
	print(mkc % (i, i, i))
	with open(asf % (i), 'w') as f:
		f.write(asc % (i))
		f.close()
