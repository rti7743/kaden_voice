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
			<input id="seach_text" type="text" value="<?= out["request"]["q"] ?>" size="50" x-webkit-speech onwebkitspeechchange=" $(this).trigger('keyup');" class="search_input" />
		</td>
	</tr>
</table>
<br />
<div class="contents" id="contents"></div>
<div class="clearfix"></div>

</body>
<script type="text/javaScript" src="jquery.js"></script>
<script type="text/javaScript">

//メイン
$(function(){
	//テンプレート
	var content_source = $("#content_source");
	//書き換えられる対象。ここにはめ込む
	var contents = $("#contents");
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

		//検索して表示
		contents.html("検索中・・・・");
		contents.load("media_search.html?search=" + encodeURIComponent(keyword) )
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