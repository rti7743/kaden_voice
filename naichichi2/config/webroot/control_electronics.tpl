<!DOCTYPE html>
<html lang="ja">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" href="./jscss/image-hover-main.css" type="text/css">
</head>
<body>

<ul class="headerbar clear">
	<li><a href="/media_start">メディア</a></li>
	<li class="selected"><a href="/electronics">家電</a></li>
	<li><a href="/setting">設定</a></li>
</ul>


<table>
	<tr>
		<td>
			<img src="./naichichi2logo.png" />
		</td>
		<td>
		</td>
	</tr>
</table>
<br />
<ul class="tab clear" id="tab">
<?lua
	for key,value in pairs(out["webmenu"]) do
?>
		<li <?lua if (value["name"] == out["nowroom"]) then ?>class="selected"<?lua end ?>><a href="?nowroom=<?= value["name"] ?>" class="blue"><?= key ?></a></li>
<?lua
	end
?>
</ul>
<br />
<br />
<div class="contents" id="contents">
<?lua
	local i = 0;
	for key,value in pairs(out["webmenu"]) do
		if (value["name"] == out["nowroom"]) then
			for menukey,menuvalue in pairs(value["menus"]) do
?>
		<div class="item">
			<div class="popupselect" style="display:none;">
<?lua
				for actionkey,actionvalue in pairs(menuvalue["actions"]) do
?>
					<span class="actionname"><a href="javascript:void(0)"><?= actionvalue["name"] ?></a></span><br />
<?lua
				end
?>
			</div>
			<div class="menuname" style="display:none;"><?= menuvalue["name"] ?></div>
			<div><a href="javascript:void(0)">
				<img width="255" height="255"  src='<?= menuvalue["image"] ?>'><br />
				<div class="status" ><?= menuvalue["status"] ?></div>
				<div class="msg" ><?= menuvalue["name"] ?></div>
			</a></div>
		</div>
<?lua
			end
		end
	end
?>
</div>
<div class="clearfix"></div><br />

</body>
<script type="text/javaScript" src="./jscss/jquery.js"></script>
<script type="text/javaScript" src="./jscss/jquery.tooltip2.js"></script>
<script type="text/javaScript">

//メイン
$(function(){
	itemaction();
});


//アイテムに動作を付ける.
function itemaction() {
	//ドキュメントの幅
	var documentwidthPer3 = (document.documentElement.scrollWidth || document.body.scrollWidth) /3;
	
	$(".item").each(function(){
		var itemObject = $(this);
		var iteminfoObject = itemObject.children(".popupselect");
		var position = itemObject.position();
		//位置によって噴出しの方向を変える
		var arrowstyle = "";
		if ( position.left  > documentwidthPer3 * 2 )
		{//右側
			arrowstyle = "bottom right";
		}
		else if ( position.left  > documentwidthPer3 )
		{//真ん中
			arrowstyle = "bottom middle";
		}
		else 
		{//左側
			arrowstyle = "bottom left";
		}
		
		//部屋名
		var roomname = $("#tab").find(".selected").text();
		//メニュー名
		var menuname = itemObject.find(".menuname").text();

		//ポップアップメニューを生成
		itemObject.tooltip2({
			 arrowtype: 'border-arrow-half2' 
			,arrowstyle: arrowstyle
			,content: iteminfoObject.html() 
			,show: function(){ this.fadeIn('slow'); }
			,hide: function(){ this.hide(); }
			,hideDelayTime: 10
		});
		//ホップアップに表示されるコンテンツ
		var contentObject = itemObject.tooltip2("content");
		//ポップアップメニューがクリックされた時の処理
		contentObject.find(".actionname a").click(function(){
			//クリックされたノードの class=item ノードわ取得する.
			var actionname = $(this).text();

			updatestats(itemObject,contentObject,actionname);
			fire(roomname,menuname,actionname);
		});

		//でかいアイコンをクリックした時に、ステータスをトグルスイッチみたいに切り替えます。
		itemObject.click(function(){
			var status = itemObject.find(".status").text();
			var newstatus = "";
			var hit = false;
			var actionname = itemObject.find(".actionname").each(function(){
				if (newstatus == "") {
					newstatus = $(this).text();
				}
				if (hit) {
					newstatus = $(this).text();
					hit = false;
					return false;
				}
				if ( $(this).text() == status ) {
					hit = true;
				}
			});
			updatestats(itemObject,contentObject,newstatus);
			fire(roomname,menuname,newstatus);
		});
	});
}

//選択されたのでステータスを更新する.
function updatestats(itemObject,contentObject,newstatus)
{
	//表示するステータスを更新する.
	itemObject.find(".status").text(newstatus);

	contentObject.find(".actionname").each(function(){
		if ($(this).text() == newstatus) {
			$(this).addClass("selected");
		}
		else {
			$(this).removeClass("selected");
		}
	});

}

//ステータスの実行
function fire(room,menu,action)
{
	$.get("electronics_fire?room=" + room + "&menu=" + menu + "&action=" + action, function(){
	});
}

</script>
</html>