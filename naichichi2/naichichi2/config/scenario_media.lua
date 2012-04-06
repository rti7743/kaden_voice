session = {};
sessionid = "myid";

function playFromNumber(number)
{
	if (not session[sessionid]) {
		return;
	}
	if (not session[sessionid]["media"]) {
		return;
	}
	if (not session[sessionid]["media"][number]) {
		return;
	}
	local media = session[sessionid]["media"][number];
	execute( media["dir"] .. "/" .. media["filename"],"" );
	return;
}

// 実行されるごとに呼ばせれます
function call()
{

	onhttp("/media_search.html",function(request){
		if ( not request["search"] ) {
			return ;
		}
		if ( not request["from"] ) {
			request["from"] = 0;
		}
		if ( not request["to"] ) {
			request["to"] = request["from"] + 50;
		}
		if ( not request["type"] ) {
			request["type"] = "all";
		}

		//検索して htmlで返す.
		local media = findmedia( request["search"],request["from"],request["to"] , request["type"]);

		local out = {}
		out["request"] = request
		if ( request["state"] == "reset") {
			session[sessionid] = {};
			session[sessionid]["media"] = {}
		}
		for k,v in pairs(media) do session[sessionid]["media"][k] = v end
		out["media"] = media;
		webload("findmedia_fragment.tpl",out);
		return ;
	});
	//メディアの画面を表示
	onhttp("/media_start",function(request){
		local out = {}
		out["request"] = request
		webload("findmedia.tpl",out);
		return ;
	});

	//メディアの画面を表示
	onhttp("/media_comm_pooling",function(request){
		webecho_table(session[sessionid] ,"json");
	});

	//メディアの再生
	onhttp("/media_play",function(request){
	print("media_play!")
		if (not request["number"]){
			return ;
		}
		playFromNumber(request["number"]);
	});

	//ページ離脱時
	onhttp("/media_pageclose",function(request){
		session[sessionid] = {};
	});

//	//音声検索
//	onvoice("([:music:])をけんさく",function(request){
//		local name = request["1"]
//		local url = gotoweb("/media_start?q=" .. name );
//		execute(url , "");
//		return ;
//	});

	//音声検索
	onvoice("おんがくけんさく",function(request){
		local url = gotoweb("/media_start");
		execute(url , "");
		return ;
	});

	//結果の番号指定での実行
	onvoice("いちばんをさいせい",function(request){
		playFromNumber("1");
		return ;
	});
	onvoice("にばんをさいせい",function(request){
		playFromNumber("2");
		return ;
	});
	onvoice("さんばんをさいせい",function(request){
		playFromNumber("3");
		return ;
	});
	onvoice("よんばんをさいせい",function(request){
		playFromNumber("4");
		return ;
	});
	onvoice("ごばんをさいせい",function(request){
		playFromNumber("5");
		return ;
	});
	onvoice("ろくばんをさいせい",function(request){
		playFromNumber("6");
		return ;
	});
	onvoice("ななばんをさいせい",function(request){
		playFromNumber("7");
		return ;
	});
	onvoice("はちばんをさいせい",function(request){
		playFromNumber("8");
		return ;
	});
	onvoice("きゅうばんをさいせい",function(request){
		playFromNumber("9");
		return ;
	});
	onvoice("じゅうばんをさいせい",function(request){
		playFromNumber("10");
		return ;
	});
	onvoice("じゅういちばんをさいせい",function(request){
		playFromNumber("11");
		return ;
	});
	onvoice("じゅうにばんをさいせい",function(request){
		playFromNumber("12");
		return ;
	});
	onvoice("じゅうさんばんをさいせい",function(request){
		playFromNumber("13");
		return ;
	});
	onvoice("じゅうよんばんをさいせい",function(request){
		playFromNumber("14");
		return ;
	});
	onvoice("じゅうごばんをさいせい",function(request){
		playFromNumber("15");
		return ;
	});
}
