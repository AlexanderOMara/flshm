package {

	import flash.text.TextField;
	import flash.text.TextFieldType;
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.display.SimpleButton;
	import flash.events.MouseEvent;
	import flash.events.StatusEvent;
	import flash.net.LocalConnection;

	public class Messenger extends Object {

		public static const WIDTH:Number = 600;
		public static const HEIGHT:Number = 400;

		public var sendText:String = 'Send';
		public var view:MovieClip = null;
		public var self:String = null;
		public var peer:String = null;
		public var peruser:Boolean = false;
		public var txtout:TextField = null;
		public var txtin:TextField = null;
		public var txtstat:TextField = null;
		public var btnsend:SimpleButton = null;
		public var btntxt:TextField = null;
		public var lc:LocalConnection = null;

		public function Messenger(view:MovieClip, self:Number, peer:Number, peruser:Boolean) {
			this.view = view;
			this.self = 'connection-' + self;
			this.peer = 'connection-' + peer;
			this.peruser = peruser;

			this.txtoutInit();
			this.txtinInit();
			this.txtstatInit();
			this.btnsendInit();
			this.connectInit();
		}

		public function txtoutInit():void {
			var txtout:TextField = new TextField();
			txtout.x = 10;
			txtout.y = 10;
			txtout.width = WIDTH - 20;
			txtout.height = HEIGHT / 2 - 40;
			txtout.multiline = true;
			txtout.border = true;
			this.view.addChild(txtout);
			this.txtout = txtout;
		}

		public function txtinInit():void {
			var txtin:TextField = new TextField();
			txtin.x = 10;
			txtin.y = HEIGHT / 2 - 20;
			txtin.width = WIDTH - 20;
			txtin.height = HEIGHT / 2 - 20;
			txtin.multiline = true;
			txtin.border = true;
			txtin.type = TextFieldType.INPUT;
			this.view.addChild(txtin);
			this.txtin = txtin;
		}

		public function txtstatInit():void {
			var txtstat:TextField = new TextField();
			txtstat.x = 10;
			txtstat.y = HEIGHT - 30;
			txtstat.width = WIDTH / 2;
			txtstat.height = 20;
			txtstat.multiline = true;
			txtstat.border = true;
			this.view.addChild(txtstat);
			this.txtstat = txtstat;
		}

		public function btnsendInit():void {
			var btnsend:SimpleButton = new SimpleButton();

			var btn:Sprite = new Sprite();
			btn.graphics.lineStyle(1, 0x000000);
			btn.graphics.beginFill(0xDDDDDD, 1);
			btn.graphics.drawRect(0, 0, 80, 20);
			btn.graphics.endFill();

			btnsend.overState = btn;
			btnsend.downState = btn;
			btnsend.upState = btn;
			btnsend.hitTestState = btn;

			btnsend.x = WIDTH - 90;
			btnsend.y = HEIGHT - 30;

			var btntxt:TextField = new TextField();
			btntxt.x = 0;
			btntxt.y = 2;
			btntxt.width = 80;
			btntxt.height = 16;
			btntxt.selectable = false;
			btntxt.text = this.sendText;
			btn.addChild(btntxt);

			btnsend.addEventListener(MouseEvent.CLICK, this.btnsendClick);

			this.btnsend = btnsend;
			this.btntxt = btntxt;
			this.view.addChild(btnsend);
		}

		public function connectInit():void {
			var lc:LocalConnection = new LocalConnection();
			if ('isPerUser' in lc) {
				lc.isPerUser = this.peruser;
			}
			var txtout:TextField = this.txtout;
			lc.client = {
				output: function(str) {
					txtout.text = str;
				}
			};
			lc.connect(this.self);
			lc.addEventListener(StatusEvent.STATUS, this.lcStatus);
			this.lc = lc;
		}

		public function btnsendClick(e:MouseEvent):void {
			this.btntxt.text = this.sendText;
			try {
				this.lc.send(this.peer, 'output', this.txtin.text);
			}
			catch (e:*) {
				this.btntxt.text = this.sendText + ': Threw';
			}
		}

		public function lcStatus(e:StatusEvent):void {
			txtstat.text = 'StatusEvent: ' +
				'level: ' + e.level +
				', code: ' + e.code;
		}
	}
}
