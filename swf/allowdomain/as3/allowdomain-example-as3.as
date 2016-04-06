package
{
	import flash.display.MovieClip;
	import flash.net.LocalConnection;
	import flash.utils.setInterval;
	import flash.events.StatusEvent

	public class main extends MovieClip {

		public function main() {
			var lc:LocalConnection = new LocalConnection();
			lc.allowDomain('example.localhost');
			lc.connect(['_connect', 'allowdomain', 'example'].join('-'));
			lc.addEventListener(StatusEvent.STATUS, function() {});
			setInterval(function() {
				var d = new Date(); 
				var r = lc.send('connection-universal', 'output', 'time:' + d.getTime());
			}, 1000);
		}
	}
}
