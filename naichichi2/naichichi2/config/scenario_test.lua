// 実行されるごとに呼ばせれます
function call()
{
	onvoice("こんにちわ" //は
		,function(e){
			speak("はい、こんにちは");
		}
	)
	onvoice("きょうのてんきわ" //は
		,function(e){
			execute("http://www3.nhk.or.jp/weather/","");
			speak("これが、今日の天気です");
		}
	)
	onvoice("ただいま|ただまー"
		,function(e){
			speak("おかえりなさい。きょうもごくろうさまです。");
			action("action__light_on");
			msleep(1000);
			action("action__display_on");
		}
	)
	onvoice("おやすみ"
		,function(e){
			speak("しょうとうモードに入ります");
			action("action__light_off");
			msleep(1000);
			action("action__display_off");
		}
	)
	onvoice("バルス"
		,function(e){
			speak("どかーん");
			action("action__light_off");
			msleep(1000);
			action("action__display_off");

			speak("めがーめがー");
		}
	)


	onvoice("(おんがくかけて|おんがくをかけて)"
		,function(e){
			execute("C:\\Users\\rti\\Desktop\\Dropbox\\library\\music\\Playlists\\いつもの.wpl","");
			speak("おんがくかけるよっ");

		 }
	)
	
	onvoice("てすと" 
		,function(e){
		}
	)
	onvoice("には" 
		,function(e){
		}
	)
//	onvoice("か|さ|た|な|ま|や|ら|わ|ん|です|ですね|ね|す|と|こ|い|う|え|お" 
//		,function(e){
//		}
//	)

	onvoice("たそがれよりもくらきものちのながれよりもあかきものわれらがまえにたちふさがりしすべてのおろかなるものにひとしくほろびをあたえんことをどらぐすれいぶ"
		,function(e){
			var_dump(e);
			speak("ばーい")
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

}
