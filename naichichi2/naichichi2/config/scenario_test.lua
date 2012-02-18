
-- 実行されるごとに呼ばせれます
function call()
	onvoice("こんにちは"
		,function(e)
			var_dump(e);
			onvoice_local("テスト"
				,function(e)
					dump("テストバック");
				 end
			);
			speak("はよー")
		 end
	)
	onvoice("さようなら"
		,function(e)
			var_dump(e);
			onvoice_local("ばーい"
				,function(e)
					dump("ばーいテストバック");
				 end
			);
			speak("ばーい")
		 end
	)
	onvoice("おんがくかけて"
		,function(e)
			execute("C:\\Users\\rti\\Desktop\\Dropbox\\library\\music\\Playlists\\いつもの.wpl","");
		 end
	)
	
	onhttp("/hoge"
		,function(e)
			e["hoge"] = "foo";
			webload("hello.tpl",e);
		end
	);
	
end
