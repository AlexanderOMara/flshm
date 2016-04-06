package
{
	import flash.display.MovieClip;
	import flash.net.LocalConnection;
	import flash.utils.setInterval;
	import flash.events.StatusEvent

	public class main extends MovieClip {

		public function main() {
			var lc:LocalConnection = new LocalConnection();
			lc.connect(['connect', 'swf20', 'as3'].join('-'));
			lc.addEventListener(StatusEvent.STATUS, function(e:*) {
			});
			setInterval(function() {
				var d:Date = new Date();
				lc.send('connection-universal', 'output', 'time:' + d.getTime());
			}, 1000);
		}
	}
}
