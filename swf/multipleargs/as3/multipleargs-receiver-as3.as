package
{
	import flash.text.TextField;
	import flash.display.MovieClip;
	import flash.net.LocalConnection;
	import flash.utils.setInterval;
	import flash.events.StatusEvent

	public class main extends MovieClip {
		
		public var txtout:TextField = null;
		public var conname:String = null;
		public var lc:LocalConnection = null;

		public function main() {
			this.conname = ['connect', 'multipleargs'].join('-');
			this.txtoutInit();
			this.connectInit();
		}

		public function txtoutInit():void {
			var txtout:TextField = new TextField();
			txtout.x = 10;
			txtout.y = 10;
			txtout.width = 600 - 20;
			txtout.height = 400 - 20;
			txtout.multiline = true;
			txtout.border = true;
			this.addChild(txtout);
			this.txtout = txtout;
		}

		public function connectInit():void {
			var lc:LocalConnection = new LocalConnection();
			var txtout:TextField = this.txtout;
			lc.client = {
				output: function(a, b, c) {
					txtout.text = a + ', ' + b + ', ' + c + '\n' + txtout.text;
				}
			};
			lc.connect(this.conname);
			this.lc = lc;
		}
	}
}
