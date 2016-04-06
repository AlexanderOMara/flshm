package
{
	import flash.text.TextField;
	import flash.display.MovieClip;
	import flash.net.LocalConnection;
	import flash.utils.setInterval;
	import flash.utils.ByteArray;

	public class main extends MovieClip {

		public function main() {
			var lc:LocalConnection = new LocalConnection();
			var n:String = ['connect', 'alltypes'].join('-');
			setInterval(function() {
				var dat:ByteArray = new ByteArray();
				dat.writeByte(0x48);
				dat.writeByte(0x65);
				dat.writeByte(0x6C);
				dat.writeByte(0x6C);
				dat.writeByte(0x6F);
				dat.writeByte(0x20);
				dat.writeByte(0x77);
				dat.writeByte(0x6F);
				dat.writeByte(0x72);
				dat.writeByte(0x6C);
				dat.writeByte(0x64);
				dat.writeByte(0x21);
				lc.send(n, 'output', dat);
			}, 1000);
		}
	}
}
