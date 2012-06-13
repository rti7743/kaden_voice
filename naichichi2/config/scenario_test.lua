// 実行されるごとに呼ばせれます
function call()
{
	onvoice("きょうのてんきわ" //は
		,function(e){
			execute("http://www3.nhk.or.jp/weather/","");
			speak("これが、今日の天気です");
		}
	)

	onvoice("(ただいまー|ただまー)"
		,function(e){
			speak("おかえりなさい。",function(){
				action("action__light_on");     // 電気をつける
				action("action__display_on");   // ディスプレイをつける

				speak("きょうも、おつかれさまです");
			});
		}
	)

	onvoice("おやすみ"
		,function(e){
			speak("しょうとうモードに、はいります。" , function(){
				action("action__display_off");
				action("action__light_off");

				speak("ぐっない。おやすみなさい。");
			});
		}
	)
	onvoice("イオナズン"
		,function(e){
		}
	)
	onvoice("メガフレア"
		,function(e){
		}
	)
	onvoice("マホートン"
		,function(e){
		}
	)
	onvoice("ホイミ"
		,function(e){
		}
	)
	onvoice("プラネタリウムモード"
		,function(e){
		}
	)
	onvoice("さくらたんのエロがぞうきぼんぬ"
		,function(e){
		}
	)
	onvoice("なのはたんのエロがぞうきぼんぬ"
		,function(e){
		}
	)
	onvoice("フェイトたんのエロがぞうきぼんぬ"
		,function(e){
		}
	)
	onvoice("バイキルト"
		,function(e){
		}
	)
	onvoice("おはよう"
		,function(e){
		}
	)
	onvoice("おはよーおはよー"
		,function(e){
		}
	)
	onvoice("ロマンチックモード"
		,function(e){
		}
	)
	onvoice("ねこみみもーど"
		,function(e){
		}
	)
	onvoice("おっぱいおっぱい"
		,function(e){
		}
	)
	onvoice("これがよのめらじゃ"
		,function(e){
		}
	)
	onvoice("バルス"
		,function(e){
		}
	)
	onvoice("ほむほむりせっと"
		,function(e){
		}
	)
	onvoice("こんなのぜったいおかしいよ"
		,function(e){
		}
	)
	onvoice("エターナルフォースブリザード"
		,function(e){
		}
	)
	onvoice("ライティング"
		,function(e){
		}
	)
	onvoice("ファイエル"
		,function(e){
		}
	)
	onvoice("きほんしせい"
		,function(e){
		}
	)

	onvoice("(おんがくかけて|おんがくをかけて)"
		,function(e){
			speak("おんがくかけるよっ");

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
