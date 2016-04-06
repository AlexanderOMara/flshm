package
{
	import flash.text.TextField;
	import flash.display.MovieClip;
	import flash.net.LocalConnection;
	import flash.utils.setInterval;
	import flash.events.StatusEvent

	public class main extends MovieClip {

		public function main() {
			var txtout:TextField = new TextField();
			txtout.x = 10;
			txtout.y = 10;
			txtout.width = 600 - 20;
			txtout.height = 400 - 20;
			txtout.multiline = true;
			txtout.border = true;
			this.addChild(txtout);

			var lc:LocalConnection = new LocalConnection();
			var n:String = ['connect', 'selfsending'].join('-');
			lc.connect(n);
			lc.client = {
				output: function(str) {
					txtout.text = str + '\n' + txtout.text;
				}
			};
			setInterval(function() {
				var d:Date = new Date();
				lc.send(n, 'output', 'time:' + d.getTime());
			}, 1000);
		}
	}
}
