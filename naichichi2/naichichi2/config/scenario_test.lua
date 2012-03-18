// 実行されるごとに呼ばせれます
function call()
{
	onvoice("こんにちは"
		,function(e){
			var_dump(e);
			onvoice_local("テスト"
				,function(e){
					dump("テストバック");
				 }
			);
			speak("はよー")
		}
	)
	onvoice("さようなら"
		,function(e){
			var_dump(e);
			onvoice_local("ばーい"
				,function(e){
					dump("ばーいテストバック");
				}
			);
			speak("ばーい")
		}
	)
	onvoice("おんがくかけて"
		,function(e){
			execute("C:\\Users\\rti\\Desktop\\Dropbox\\library\\music\\Playlists\\いつもの.wpl","");
		 }
	)
	

}
