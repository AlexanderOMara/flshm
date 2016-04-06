package
{
	import flash.text.TextField;
	import flash.display.MovieClip;
	import flash.net.LocalConnection;
	import flash.utils.setInterval;
	import flash.events.StatusEvent

	public class main extends MovieClip {

		public function main() {
			var lc:LocalConnection = new LocalConnection();
			var n:String = ['connect', 'multipleargs'].join('-');
			setInterval(function() {
				var d:Date = new Date();
				lc.send(n, 'output', true, 'two', d.getTime());
			}, 1000);
		}
	}
}
