package
{
	import flash.text.TextField;
	import flash.display.MovieClip;

	import Messenger;

	public class main extends MovieClip {

		public function main() {
			new Messenger(this, 1, 2, false);
		}
	}
}
