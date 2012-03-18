<!DOCTYPE html>
<html lang="ja">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" href="./image-hover-main.css" type="text/css">
</head>
<body>

<table>
	<tr>
		<td>
			<img src="./naichichi2logo.png" />
		</td>
		<td>
			<input id="seach_text" type="text" value="<?= out["request"]["q"] ?>" size="50" x-webkit-speech onwebkitspeechchange="g_page = 1; $(this).trigger('keyup');" class="search_input" />
		</td>
	</tr>
</table>
<br />
<div class="contents" id="contents"></div>
<div class="clearfix"></div><br />
<div class="message" id="message"></div><br />

</body>
<script type="text/javaScript" src="jquery.js"></script>
<script type="text/javaScript">

//現在のページ数
var g_page = 1;

//メイン
$(function(){

	//ドキュメントの幅
	var documentwidth = document.documentElement.scrollWidth || document.body.scrollWidth;
	//ドキュメントの高さ
	var documentheight = document.documentElement.scrollHeight || document.body.scrollHeight;
	//自動読み込みする場所(マイナスの場合は自動読み込みしない)
	var autoloadFirePoint = documentheight - $(window).height();
	
	//横に何個おけるか？
	var countOfWidth = Math.floor(documentwidth / 300);
	if (countOfWidth <= 0) countOfWidth = 1;
	//1ページに何行表示するか？
	var countOfPage = countOfWidth * 5;
	//テンプレート
	var content_source = $("#content_source");
	//書き換えられる対象。ここにはめ込む
	var contents = $("#contents");
	//メッセージを出す所
	var message = $("#message");
	//画面自体を書き換えるサジェストをします。
	var seach_text = $("#seach_text");

	var oldkeyword;
	var lastkeyword = "--last--";
	seach_text.keyup(function(){
		var keyword = seach_text.val();
		if (lastkeyword == keyword){
			return ; //キーワードが変わっていないので無視
		}
		lastkeyword = keyword;

		//多重に読み込まないように、マイナス値に変える.
		autoloadFirePoint = -1;
		//読み込み直すので pageを1にする
		g_page = 1;

		contents.html("");
		message.html("検索中・・・・");
		var from = 0;
		var to = countOfPage;
		$.get("media_search.html?search=" + encodeURIComponent(keyword) + "&from=" + from + "&to=" + to + "&state=reset" , function(reshtml){
			if (reshtml == ""){
				message.html("みつかりませんでした。");
			}
			else {
				contents.html(reshtml);
				message.html("");

				//コンテンツを追加したので、ドキュメント高さを再計測.

				//ドキュメントの高さ
				documentheight = document.documentElement.scrollHeight || document.body.scrollHeight;
				//自動読み込みする場所(マイナスの場合は自動読み込みしない)
				autoloadFirePoint = documentheight - $(window).height();
			}
		});
	});
	//スクロールで最後まで行ったら再読み込みをかけます。
	$(window).scroll( function(){
		if ( autoloadFirePoint <= 0 || $(document).scrollTop() < autoloadFirePoint){
			return ;
		}
		//下まで行ったので自動読み込みする
		
		//多重に読み込まないように、マイナス値に変える.
		autoloadFirePoint = -1;
		g_page ++;

		var keyword = seach_text.val();

		var from = (g_page - 1) * countOfPage;
		var to = g_page * countOfPage;
		if (from <= 0 || to < from )
		{
			from = 0;
			to = countOfPage;
			page = 1;
		}
		message.html("読み込み中・・・・");
		$.get("media_search.html?search=" + encodeURIComponent(keyword) + "&from=" + from + "&to=" + to + "&state=next" , function(reshtml){
			if (reshtml == ""){
				message.html("これ以上ページはありません");
			}
			else {
				contents.append(reshtml);
				message.html("");

				//コンテンツを追加したので、ドキュメント高さを再計測.

				//ドキュメントの高さ
				documentheight = document.documentElement.scrollHeight || document.body.scrollHeight;
				//自動読み込みする場所(マイナスの場合は自動読み込みしない)
				autoloadFirePoint = documentheight - $(window).height();
			}
		});
	});

	//検索窓にフォーカスを合わせる.
	seach_text.get(0).focus();

	//ページをロードした時にも検索かけます。
	seach_text.trigger('keyup');
	
});
function mediaplay(number) {
	$.get("media_play?number=" + number, function(){
	});
}
</script>
</html>