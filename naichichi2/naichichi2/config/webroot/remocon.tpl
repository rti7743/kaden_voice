<!DOCTYPE html>
<html lang="ja"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta charset="utf-8">
	<meta name="viewport" content="width=640" />
	<title>jQuery UI Sortable - Display as grid</title>
	<base href="/"></base>
	<link type="text/css" href="./jscss/jquery.mobile.css" rel="stylesheet" />
	<link type="text/css" href="./jscss/ui-lightness/jquery-ui-1.8.21.custom.css" rel="stylesheet" />
	<script type="text/javascript" src="./jscss/jquery.js"></script>
	<script type="text/javascript" src="./jscss/jquery-ui.js"></script>
	<script type="text/javascript" src="./jscss/jquery.mobile.js"></script>
	<script type="text/javaScript" src="./jscss/jquery.tooltip2.js"></script>

	<style>

	#remoconmenu_ul {
		list-style-type: none;
		margin: 10;
		padding: 10; 
	}
	#remoconmenu_ul li {
		margin: 15px 15px 15px 15px;
		padding: 0px;
		float: left;
		width: 200px;
		height: 200px;
		border: 1px solid #CCCCCC;
		color: #1C94C4;
   	}

	#remoconmenu_ul .itembox {
		position:	relative;
	}

	#remoconmenu_ul .delicon {
		z-index:	99;
		font-size:	10px;
		position:	absolute;
		top:		0px;
		left:		0px;
	}
	#remoconmenu_ul .editicon {
		z-index:	99;
		font-size:	10px;
		position:	absolute;
		top:		90px;
		left:		120px;
	}
	#remoconmenu_ul .mainicon {
		z-index:	1;
		position:	absolute;
		top:		5px;
		left:		10px;
		width:		128px;
		height:		128px;
	}
	#remoconmenu_ul .title {
		z-index:	10;
		font-size:	32px;
		position:	absolute;
		top:		130px;
		left:		5px;
	}
	#remoconmenu_ul .status {
		z-index:	10;
		font-size:	20px;
		position:	absolute;
		top:		165px;
		left:		5px;
	}

	.clearfix{
		clear:both;
		float:none;
	}
	.icon {
		z-index: 99;
		font-size: small;
	}
	.showeditmode {
		display:	none;
	}
	.editicon {
	}
	.menubutton {
	}
	
	.error {
		display: none;
		font-color: #red;
	}
	.notify {
		display: none;
		font-color: #yellow;
	}

	.js_template {
		display: none;
	}
	
	.contents_wrapper {
		width:980px;
		margin:0 auto;
	}

	@media screen and (max-device-width: 640px), screen and (max-width: 640px) {
		.contents_wrapper {
			width:640px;
			margin:0 auto;
		}
	}
	</style>
	<script>
		//jquery mobile でリロードした時にデータが引き継がれない問題を強引に何とかする・・・(もっといい方法はあるはず)
		if ( window.location.href.match(/#/) )
		{
			window.location.href = window.location.href.replace(/#.*$/,"");
		}
	</script>
</head>
<body class="ui-mobile-viewport ui-overlay-c"> 

<div data-role="page" id="top" data-url="top" tabindex="0" data-theme="c" class="ui-page ui-body-c ui-page-active">
	<div data-role="header"> 
		<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
	</div> 

	<div data-role="content" data-theme="c" class="ui-content contents_wrapper" role="main">
	<p><a data-role="button" class="menubutton" data-inline="true">設定→</a><a data-role="button" class="menubutton" data-inline="true">リモコン</a></p>

	<ul id="remoconmenu_li_sample" class="js_template">
		<li class="ui-state-default item">
			<div class="itembox">
				<span class="key" style="display: none;">%KEY%</span>
				<a href="" data-role="button" data-icon="delete" class="icon delicon" data-iconpos="notext"></a>
				<div class="mainicon"><img src="1345449140_ktip.png" /></div>
				<div class="title">%NAME%</div>
				<div class="status">%STATUS%</div>
				<a href="" data-role="button" data-icon="false" class="icon editicon"><span>edit</span></a>
				<div class="popupselect" style="display:none;">
					%POPUPSELECT%
				</div>
			</div>
		</li>
	</ul>
	<ul id="remoconmenu_popup_sample" class="js_template">
					<a href="javascript:void(0)">
						<span class="actionkey" style="display: none;">%ACTIONKEY%</span>
						<span class="actionname">%ACTIONNAME%</span>
						<br />
					</a>
	</ul>
	<ul id="remoconmenu_ul" class="ui-sortable">
	</ul>
	<br class="clearfix">

	<p><a class="editable" data-role="button" onclick="OnEditbalePageOpen('new');" >新規作成する</a></p>

	<div data-role="fieldcontain">
		<fieldset data-role="controlgroup">
			<input type="checkbox" name="gotoedit" id="gotoedit" value="1" />
			<label for="gotoedit">編集する</label>
		</fieldset>
	</div>

	</div><!-- /content -->

	<div data-role="footer" data-position="fixed">
		<h4>(C) 2012 rti Institute of Technology</h4> 
	</div> <!-- /footer -->
</div><!-- /page top -->

<div data-role="page" id="editable" data-theme="c" data-url="editable" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">
	<div data-role="header"> 
		<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error" id="editable_error">エラーが発生しました!!</div>
	<p><a data-role="button" class="menubutton" data-inline="true">設定→</a><a data-role="button" class="menubutton" data-inline="true">リモコン→</a><a data-role="button" class="menubutton" data-inline="true">設定1</a></p>

	<input type="hidden" id="editable_key" value="">
	<dl>
	<dt>名前</dt>
	<dd>
		<input type="text" name="name" id="name" value="">
		<div class="error" id="name_error">名前が入力されていません</div>
	</dd>
	<dt>種類</dt>
	<dd>
		<select name="type" id="type" size="1">
		<option value="エアコン">エアコン</option>
		<option value="テレビ">テレビ</option>
		<option value="照明">照明</option>
		<option value="プロジェクタ">プロジェクタ</option>
		<option value="レコーダ">レコーダ</option>
		<option value="パソコン">パソコン</option>
		<option value="ディスプレイ">ディスプレイ</option>
		<option value="その他">その他</option>
		</select>
		<div id="type_other_div">
		その他種類名:<input type="text" name="type_other"  id="type_other" value="">
		<div class="error" id="type_other_error">その他の名前を入れてください。</div>
		</div>
	</dd>
	<dt>動作</dt>
	<dd>
		<ul id="actionmenu_li_sample" class="js_template">
			<li>
				<span style="display: none;" class="id">%ACTIONKEY%</span>
				<a href="javascript:void(0)" onclick="OnActionEditablePageOpen($('#editable_key').val(),'%ACTIONKEY%');"><span class="name">%ACTIONNAME%</span></a>
			</li>
		</ul>
		<ul id="actionmenu_ul" class="ui-sortable">
		</ul>
		<p><a data-role="button" onclick="OnActionEditablePageOpen($('#editable_key').val(),'new');" >動作を新規に追加する</a></p>
	</dd>
	</dl>

	<br />
	<br />
	<p><a data-role="button" onclick="remocon_update_elec($('#editable_key').val());" >保存して戻る</a></p>
	</div><!-- /content -->

	<div data-role="footer" data-position="fixed">
		<h4>(C) 2012 rti Institute of Technology</h4> 
	</div> <!-- /footer -->
</div><!-- /page editable -->


<div data-role="page" id="actioneditable" data-theme="c" data-url="actioneditable" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">
	<div data-role="header"> 
		<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error" id="actioneditable_error">エラーが発生しました!!</div>

	<p><a data-role="button" class="menubutton" data-inline="true">設定→</a><a data-role="button" class="menubutton" data-inline="true">リモコン→</a><a data-role="button" class="menubutton" data-inline="true">設定1→</a><a data-role="button" class="menubutton" data-inline="true">設定2</a></p>

	<input type="hidden" id="actioneditable_key1" value="">
	<input type="hidden" id="actioneditable_key2" value="">
	<dl>
	<dt>動作名称</dt>
	<dd>
		<input type="text" name="actionname" id="actionname" value="">
		<div class="error" id="actionname_error">動作の名前を入れてください。</div>
	</dd>
	<dt>種類</dt>
	<dd>
		<select name="actiontype" id="actiontype" size="1">
		<option value="つける">つける</option>
		<option value="けす">けす</option>
		<option value="小">小</option>
		<option value="中">中</option>
		<option value="大">大</option>
		<option value="つぎの">つぎの</option>
		<option value="まえの">まえの</option>
		<option value="取り出し">取り出し</option>
		<option value="一覧表">一覧表</option>
		<option value="1ch">1ch</option>
		<option value="2ch">2ch</option>
		<option value="3ch">3ch</option>
		<option value="4ch">4ch</option>
		<option value="5ch">5ch</option>
		<option value="6ch">6ch</option>
		<option value="7ch">7ch</option>
		<option value="8ch">8ch</option>
		<option value="9ch">89ch</option>
		<option value="10ch">10ch</option>
		<option value="11ch">11ch</option>
		<option value="12ch">12ch</option>
		<option value="外部接続1">外部接続1</option>
		<option value="外部接続2">外部接続2</option>
		<option value="音量ミュート">音量ミュート</option>
		<option value="音量ミュート解除">音量ミュート解除</option>
		<option value="その他">その他</option>
		</select>
		<div id="actiontype_other_div">
		その他種類名:<input type="text" name="actiontype_other" name="actiontype_other" value="">
		<div class="error" id="actiontype_other_error">その他の動作名を入れてください。</div>
		</div>
	</dd>
	<dt>操作方法</dt>
	<dd>
		<label><input type="checkbox" name="actionvoice" id="actionvoice">音声認識する</label>
			<div id="actionvoice_div">
			音声認識コマンド:<input type="text" id="actionvoice_command" name="actionvoice_command" value="">
			<div class="error" id="actionname_error">音声認識するコマンドを入れてください。<br />
			例: テレビをつけて<br />
			例: でんきけして|でんきけしてください|でんきけしてござらぬか<br />
			</div>
			</div>
			<br />
		<label><input type="checkbox" id="showremocon">リモコン画面に表示する</label>
		<label><input type="checkbox" id="useapi">API経由での操作を許可する</label>
		<label><input type="checkbox" id="useinternet">インターネットからの操作を許可する</label>
		<label><input type="checkbox" id="tospeak">実行時にしゃべる</label>
			<div id="tospeak_div">
			<select name="tospeak_select" id="tospeak_select" size="1">
			<option value="文字列">文字列</option>
			<option value="音楽ファイル">音楽ファイル</option>
			</select>
				<div id="tospeak_string_div">
					読み上げる文字列<br />
					<input type="text" id="tospeak_string" name="tospeak_string" value=""><br />
					<div class="error" id="tospeak_string_error">読み上げる文字列を入力してください。</div>
				</div>
				<div id="tospeak_mp3_div">
					読み上げる音楽ファイル (.wav // .mp3 // .ogg などが指定できます。)<br />
					<input type="text" id="tospeak_mp3" name="tospeak_mp3" value=""><br />
					<div class="error" id="tospeak_mp3_error">読み上げるファイル名を入れてください。</div>
				</div>
			</div>
			<br />
	</dd>
	<dt>操作方法</dt>
	<dd>
		<select id="actionexecuteflag" size="1">
		<option value="赤外線">赤外線</option>
		<option value="赤外線プリセット">赤外線プリセット</option>
		<option value="コマンド">コマンド</option>
		<option value="何もしない">何もしない</option>
		</select>
		<br />
		<div id="actionexecuteflag_ir_div">
		<dl>
			<dt>学習<dt>
			<dd>
				<input type="button" id="actionexecuteflag_ir_leaning" value="赤外線を学習させる"><br />
				<input type="hidden" id="actionexecuteflag_ir" name="actionexecuteflag_ir" value=""><br />
				<div id="actionexecuteflag_ir_registlabel">現在登録されています。</div>
				<div class="error" id="actionexecuteflag_ir_error">リモコン学習がされていません。<br />
				学習は、「赤外線を学習させる」ボタンを押した後で、機材に向かって、リモコンの信号を送信してください<br />
				</div>
				<div class="error" id="actionexecuteflag_ir_error2">学習に失敗しました<br />
				リモコンコードの学習に失敗しました。<br />
				</div>
			</dd>
			<dt>送信周波数<dt>
			<dd>
				<select name="actionexecuteflag_ir_freq" size="1">
				<option value="38">38khz</option>
				<option value="39">39khz</option>
				<option value="40">40khz</option>
				<option value="37">37khz</option>
				<option value="36">36khz</option>
				</select>
			</dd>
		</dl>
		</div>
		<div id="actionexecuteflag_command_div">
		<dl>
			<dt>コマンド<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command" name="actionexecuteflag_command" value="コマンドを指定する">
				<div class="error" id="actionexecuteflag_command_error">コマンドを指定してください<br />
				登録されているコマンドについては、設定→コマンド　から確認してください。<br />
				</div>
			</dd>
			<dt>引数1<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args1" name="actionexecuteflag_command_args1" value="">
			</dd>
			<dt>引数2<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args2" name="actionexecuteflag_command_args2" value="">
			</dd>
			<dt>引数3<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args3" name="actionexecuteflag_command_args3" value="">
			</dd>
			<dt>引数4<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args4" name="actionexecuteflag_command_args4" value="">
			</dd>
			<dt>引数5<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args5" name="actionexecuteflag_command_args5" value="">
			</dd>
		</dl>
		</div>
	</dd>
	</dl>

	<br />
	<br />
	<p><a data-role="button" onclick="remocon_update_elec_action(  $('#actioneditable_key1').val()  ,  $('#actioneditable_key2').val()  );" >保存して戻る</a></p>

	</div><!-- /content -->

	<div data-role="footer" data-position="fixed">
		<h4>(C) 2012 rti Institute of Technology</h4> 
	</div> <!-- /footer -->
</div><!-- /page actioneditable -->


<script>

var g_Remocon = %REMOCON%;
var g_enableEditMode = false;


//アイテムに動作を付ける.
function itemaction() {
	//ドキュメントの幅
	var documentwidthPer3 = (document.documentElement.scrollWidth || document.body.scrollWidth) /3;

	var htmltemplate = $("#remoconmenu_li_sample").html();
	var htmltemplatepopup = $("#remoconmenu_popup_sample").html();
	var totaldom = "";
	for(var arraykey in g_Remocon)
	{
		var s = arraykey.split("_");
		if ( !(s[0] == "elec" && s[2] == "name")) continue;
		var key = s[1];

		var html = htmltemplate;
		html = html.replace(/%KEY%/g , key);
		html = html.replace(/%NAME%/g , g_Remocon["elec_" + key + "_name"]);
		html = html.replace(/%STATUS%/g , g_Remocon["elec_" + key + "_status"]);

		var totalhtmlpopup = "";
		for(var arraykey2 in g_Remocon)
		{
			var s = arraykey2.split("_");
			if ( !(s[0] == "elec" && s[1] == key && s[2] == "action" && s[4] == "name")) continue;
			var actionkey = s[3];

			var htmlpopup = htmltemplatepopup;
			htmlpopup = htmlpopup.replace(/%ACTIONKEY%/g , actionkey);
			htmlpopup = htmlpopup.replace(/%ACTIONNAME%/g , g_Remocon["elec_" + key + "_action_" + actionkey + "_name"]);
			totalhtmlpopup += htmlpopup;
		}
		html = html.replace(/%POPUPSELECT%/g , totalhtmlpopup);
		totaldom += html;
	}
	$("#remoconmenu_ul").html(totaldom);


	$("#remoconmenu_ul").find("li").each(function(){
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

//		//部屋名
//		var roomname = $("#tab").find(".selected").text();
		//メニュー
		var menukey = itemObject.find(".key").text();
		var menuname = itemObject.find(".name").text();

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
		contentObject.find("a").click(function(){
			var actionkey  = $(this).children(".actionkey").text();
			var actionname = $(this).children(".actionname").text();

			updatestats(itemObject,contentObject,actionname);
			fire(menuname,actionname);
		});

		//でかいアイコンをクリックした時に、ステータスをトグルスイッチみたいに切り替えます。
		itemObject.click(function(){
			if (g_enableEditMode) {
				return false;
			}
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
			fire(menuname,newstatus);
			return false;
		});

		//削除アイコンがクリックされた時の動作
		itemObject.find(".delicon").click(function(){
			alert("削除");
			return false;
		});

		//編集アイコンがクリックされた時の動作
		itemObject.find(".editicon").click(function(){
			OnEditbalePageOpen(menukey);
			return true;
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
function remocon_fire_byid(key1,key2)
{
	$.get("/remocon/fire/byid?key1=" + key1 + "&key2=" + key2, function(){
	});
}
function remocon_fire_byname(name1,name2)
{
	$.get("/remocon/fire/byname?name1=" + name1 + "&name2=" + name2, function(){
	});
}


//並び順の更新
function remocon_update_icon_order(menu,action)
{
	var orderbyArray = {};

	var orderby = 1;
	$("#remoconmenu_ul").find("li").each(function(){
		var itemObject = $(this);
		var menukey = itemObject.find(".key").text();
		var keyString = "elec_" + menukey + "_order";

		orderbyArray[keyString] = orderby++;
	});

	//elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&
	$.post("/remocon/update/icon_order",orderbyArray, function(){
	});
}
//actionの並び順の更新
function remocon_update_elec_action_order(menu,action)
{
	var orderbyArray = {};

	var orderby = 1;
	var editable_key = $("#editable_key").val();
	$("#actionmenu_ul").find("li").each(function(){
		var itemObject = $(this);
		var actionkey = itemObject.find(".id").text();
		var keyString = "elec_" + editable_key + "_action_" + actionkey + "_order";

		orderbyArray[keyString] = orderby++;
	});

	//elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge
	$.post("/remocon/update/elec_action_order",orderbyArray, function(){
	});
}

function remocon_update_elec(key)
{
	if ($("#editable_key").val() != key)
	{
		alert("editableページが壊れています。キーと現在編注中のデータが一致しません");
		return false;
	}
	
	var updateArray = {};
	var bad = false;
	updateArray["editable_key"] = key;
	updateArray["name"] = $("#name").val();
	updateArray["type"] = $("#type").val();
	updateArray["type_other"] = $("#type_other").val();
	if (updateArray["name"] == "")
	{
		$("#name_error").show();
		bad = true;
	}
	if (updateArray["type"] == "その他" && updateArray["type_other"] == "")
	{
		$("#type_other_error").show();
		bad = true;
	}


	if ( bad )
	{//エラー
		$("#editable_error").show();
		return false;
	}
	
	//サーバにデータを送信
	//editable_key=123&name=名前&type=タイプ&type_other=タイプその他
	$.post("/remocon/update/elec",updateArray, function(){
	});
	
	//データを更新
	return true;
}

function remocon_update_elec_action(parentkey,key)
{
	if ($("#actioneditable_key1").val() != parentkey || $("#actioneditable_key2").val() != key)
	{
		alert("actioneditableページが壊れています。キーと現在編注中のデータが一致しません");
		return false;
	}
	
	var updateArray = {};
	var bad = false;
	updateArray["actioneditable_key1"] = parentkey;
	updateArray["actioneditable_key2"] = key;
	updateArray["actionname"] = $("#actionname").val();
	updateArray["actiontype"] = $("#actiontype").val();
	updateArray["actiontype_other"] = $("#actiontype_other").val();
	updateArray["actionvoice"] = $("#actionvoice").val();
	updateArray["actionvoice_command"] = $("#actionvoice_command").val();
	updateArray["showremocon"] = $("#showremocon").val();
	updateArray["useapi"] = $("#useapi").val();
	updateArray["useinternet"] = $("#useinternet").val();
	updateArray["tospeak"] = $("#tospeak").val();
	updateArray["tospeak_select"] = $("#tospeak_select").val();
	updateArray["tospeak_string"] = $("#tospeak_string").val();
	updateArray["tospeak_mp3"] = $("#tospeak_mp3").val();

	updateArray["actionexecuteflag"] = $("#actionexecuteflag").val();
	updateArray["actionexecuteflag_ir"] = $("#actionexecuteflag_ir").val();
	updateArray["actionexecuteflag_command"] = $("#actionexecuteflag_command").val();
	updateArray["actionexecuteflag_command_args1"] = $("#actionexecuteflag_command_args1").val();
	updateArray["actionexecuteflag_command_args2"] = $("#actionexecuteflag_command_args2").val();
	updateArray["actionexecuteflag_command_args3"] = $("#actionexecuteflag_command_args3").val();
	updateArray["actionexecuteflag_command_args4"] = $("#actionexecuteflag_command_args4").val();
	updateArray["actionexecuteflag_command_args5"] = $("#actionexecuteflag_command_args5").val();

	if (updateArray["actionname"] == "")
	{
		$("#actionname_error").show();
		bad = true;
	}
	if (updateArray["actiontype"] == "その他" && updateArray["actiontype_other"] == "")
	{
		$("#actiontype_other_error").show();
		bad = true;
	}

	if (updateArray["actionvoice"] && updateArray["actionvoice_command"]  == "")
	{
		$("#actionvoice_command_error").show();
		bad = true;
	}

	if (updateArray["tospeak"])
	{
		if (updateArray["tospeak_select"] == "文字列")
		{
			if (updateArray["tospeak_string"] == "")
			{
				$("#tospeak_string_error").show();
				bad = true;
			}
		}
		else if (updateArray["tospeak_select"] == "音楽ファイル")
		{
			if (updateArray["tospeak_mp3"] == "")
			{
				$("#tospeak_mp3_error").show();
				bad = true;
			}
		}
	}

	if (updateArray["actionexecuteflag"] == "赤外線")
	{
		if (updateArray["actionexecuteflag_ir"] == "")
		{
			$("#actionexecuteflag_ir_error").show();
			bad = true;
		}
	}
	else if (updateArray["actionexecuteflag"] == "コマンド")
	{
		if (updateArray["actionexecuteflag_command"] == "")
		{
			$("#actionexecuteflag_command_error").show();
			bad = true;
		}
	}

	if ( bad )
	{//エラー
		$("#editable_error").show();
		return false;
	}
	
	//サーバにデータを送信

	//actioneditable_key1=123&actioneditable_key2=456&actionname=名前&actiontype=種類&actiontype_other=種類その他&actionvoice=1&actionvoice_command=こんぴゅーた&showremocon=1&useapi=1&useinternet=1&tospeak_select=ターゲット&tospeak_string=読み上げ文字列&tospeak_mp3=hoge.mp3&actionexecuteflag=赤外線&actionexecuteflag_ir=12345678910&actionexecuteflag_command=a.exe&actionexecuteflag_command_args1=&actionexecuteflag_command_args2=&actionexecuteflag_command_args3=&actionexecuteflag_command_args4=&actionexecuteflag_command_args5=
	$.post("/remocon/update/elec_action",updateArray, function(){
	});
	
	//データを更新
	return true;
}



function OnEditbalePageOpen(key)
{
	//いたづら防止のために補正する
	key = parseIntOrNew(key);

	//エラーを消す.
	$("#editable .error").hide();
	//通知も消す
	$("#editable .notify").hide();

	$("#actionmenu_ul").html("");
	$("#editable_key").val(key);

	if (key == "new")
	{//新規作成
		$("#name").val("");
		$("#type").val("--");
		$("#type_other_div").hide();
		$("#type_other").val("");
	}
	else
	{//新規作成でないので、データを取得してくる.
		if (! g_Remocon["elec_" + key + "_name"] )
		{
			//データがない!!
			alert("指定されたデータが見つかりません。削除されている可能性があります。");
			return ;
		}
		$("#name").val(g_Remocon["elec_" + key + "_name"]).change();
		$("#type").val(g_Remocon["elec_" + key + "_type"]).change();

		var htmltemplate = $("#actionmenu_li_sample").html();
		var totaldom = "";
		for(var arraykey2 in g_Remocon)
		{
			var s = arraykey2.split("_");
			if ( !(s[0] == "elec" && s[1] == key && s[2] == "action" && s[4] == "name")) continue;
			var actionkey = s[3];

			var html = htmltemplate;
			html = html.replace(/%ACTIONKEY%/g,actionkey);
			html = html.replace(/%ACTIONNAME%/g,g_Remocon["elec_" + key + "_action_" + actionkey + "_name"]);
			totaldom += html;
		}
		$("#actionmenu_ul").html(totaldom);
	}
	$.mobile.changePage("#editable", { transition: "flip" });
}

function getElecText(value,key1,key2)
{
	if (key2 == undefined)
	{
		var v = g_Remocon["elec_" + key1 + "_" + value ];
		if ( v == undefined )
		{
			return "";
		}
		return v;
	}

	var v = g_Remocon["elec_" + key1 + "_action_" + key2 + "_" + value ];
	if ( v == undefined )
	{
		return "";
	}
	return v;
}

function BoolToChcked(obj , boolString)
{
	if (boolString)
	{
		 obj.attr('checked','checked');
	}
	else
	{
		 obj.removeAttr('checked');
	}
	return obj;
}
function parseIntOrNew(v)
{
	if (v == "new")
	{
		return "new";
	}
	return parseInt(v);
}

function OnActionEditablePageOpen(parentkey,key)
{
	//いたずら防止のために補正する.
	parentkey = parseInt(parentkey);
	key = parseIntOrNew(key);

	$("#actioneditable_key1").val(parentkey).change();
	$("#actioneditable_key2").val(key).change();

	//エラーを消す.
	$("#actioneditable .error").hide();
	//通知も消す
	$("#actioneditable .notify").hide();

	if (key == "new")
	{//新規作成
		$("#actionname").val("").change();	//名前
		$("#actiontype").val("").change();	//種類

		BoolToChcked( $("#actionvoice") ,1).change();	//音声認識する
		$("#actionvoice_command").val("").change();
		BoolToChcked( $("#showremocon") ,1).change();	//リモコン画面に表示する
		BoolToChcked( $("#useapi") ,1).change();			//APIからの操作を許可する
		BoolToChcked( $("#useinternet") ,1).change();	//インターネットからの操作を許可する
		BoolToChcked( $("#tospeak") ,1).change();		//実行時にしゃべる

		$("#tospeak_select").val("文字列").change();
		$("#tospeak_string").val("${家電名}を${操作名}します").change();
		$("#tospeak_mp3").val("").change();

		$("#actionexecuteflag").val("赤外線").change();		//操作方法
		
		$("#actionexecuteflag_command").val("").change();			//コマンド本体
		$("#actionexecuteflag_command_args1").val("").change();		//引数1
		$("#actionexecuteflag_command_args2").val("").change();		//引数2
		$("#actionexecuteflag_command_args3").val("").change();		//引数3
		$("#actionexecuteflag_command_args4").val("").change();		//引数4
		$("#actionexecuteflag_command_args5").val("").change();		//引数5
	}
	else
	{//新規作成でないので、データを取得してくる.
		key = parseInt(key);
		if ( getElecText("name",parentkey,key) == "")
		{
			//データがない!!
			alert("指定されたデータが見つかりません。削除されている可能性があります。");
			return ;
		}

		$("#actionname").val( getElecText("name",parentkey,key) ).change();
		$("#actiontype").val( getElecText("type",parentkey,key) ).change();
		$("#actiontype_other_div").val( getElecText("type_other",parentkey,key) ).change();

		
		BoolToChcked( $("#actionvoice") , getElecText("actionvoice",parentkey,key) ).change();			//音声認識する
		$("#actionvoice_command").val( getElecText("actionvoice_command",parentkey,key) ).change();		//音声認識のコマンド

		BoolToChcked( $("#showremocon") ,getElecText("showremocon",parentkey,key) ).change();			//リモコン画面に表示する
		BoolToChcked( $("#useapi")      ,getElecText("useapi",parentkey,key) ).change();				//APIからの操作を許可する
		BoolToChcked( $("#useinternet") ,getElecText("useinternet",parentkey,key) ).change();			//インターネットからの操作を許可する
		BoolToChcked( $("#tospeak") ,getElecText("tospeak",parentkey,key) ).change();					//実行時にしゃべる

		$("#tospeak_select").val(getElecText("tospeak_select",parentkey,key)).change();					//読み上げ方法
		$("#tospeak_string").val(getElecText("tospeak_string",parentkey,key)).change();					//文字列読み上げ
		$("#tospeak_mp3").val(getElecText("tospeak_mp3",parentkey,key)).change();						//音楽ファイル読み上げ
		$("#tospeak").change();

		$("#actionexecuteflag").val(getElecText("actionexecuteflag",parentkey,key)).change();			//操作方法
		$("#actionexecuteflag_ir").val(getElecText("actionexecuteflag_ir",parentkey,key)).change();
		$("#actionexecuteflag_command").val(getElecText("actionexecuteflag_command",parentkey,key)).change();
		$("#actionexecuteflag_command_args1").val(getElecText("actionexecuteflag_command_args1",parentkey,key)).change();		//引数1
		$("#actionexecuteflag_command_args2").val(getElecText("actionexecuteflag_command_args2",parentkey,key)).change();		//引数2
		$("#actionexecuteflag_command_args3").val(getElecText("actionexecuteflag_command_args3",parentkey,key)).change();		//引数3
		$("#actionexecuteflag_command_args4").val(getElecText("actionexecuteflag_command_args4",parentkey,key)).change();		//引数4
		$("#actionexecuteflag_command_args5").val(getElecText("actionexecuteflag_command_args5",parentkey,key)).change();		//引数5
	}

	$.mobile.changePage("#actioneditable", { transition: "slide" });
}

$(function() {
	//jquery ui sorttable を構築
	$( "#remoconmenu_ul" ).sortable();
	$( "#actionmenu_ul" ).sortable();

	//家電アイコンに動作を付ける.
	itemaction();

	//編集モード
	$("#gotoedit").change(function(){
		if ( $(this).attr('checked') ) {
			$( "#remoconmenu_ul" ).sortable( "enable" );
			$( ".editable" ).show();
			$( ".delicon" ).show();
			$( ".realdelicon" ).hide();
			$( ".editicon" ).show();
			$("#remoconmenu_ul").find("li").each(function(){
				var itemObject = $(this);
				itemObject.tooltip2("disable");
			});

			g_enableEditMode = true;
		} else {
			$( "#remoconmenu_ul" ).sortable( "disable" );
			$( ".editable" ).hide();
			$( ".delicon" ).hide();
			$( ".realdelicon" ).hide();
			$( ".editicon" ).hide();
			$("#remoconmenu_ul").find("li").each(function(){
				var itemObject = $(this);
				itemObject.tooltip2("enable");
			});

			g_enableEditMode = false;
		}
	});

	//編集画面のイベント
	$("#type").change(function(){
		if ( $(this).val() == "その他" )
		{
			$("#type_other_div").show();
		}
		else
		{
			$("#type_other_div").hide();
		}
	});

	$("#actiontype").change(function(){
		if ( $(this).val() == "その他" )
		{
			$("#actiontype_other_div").show();
		}
		else
		{
			$("#actiontype_other_div").hide();
		}
	});

	$("#actionvoice").change(function(){
		if ( $(this).attr('checked') )
		{
			$("#actionvoice_div").show();
		}
		else
		{
			$("#actionvoice_div").hide();
		}
	});

	$("#tospeak").change(function(){
		if ( $(this).attr('checked') )
		{
			$("#tospeak_div").show();
		}
		else
		{
			$("#tospeak_div").hide();
		}
	});

	$("#tospeak_select").change(function(){
		if ( $("#tospeak_select").val() == "文字列" )
		{
			$("#tospeak_mp3_div").hide();
			$("#tospeak_string_div").show();
		}
		else if ( $("#tospeak_select").val() == "音楽ファイル" )
		{
			$("#tospeak_string_div").hide();
			$("#tospeak_mp3_div").show();
		}
	});

	$("#actionexecuteflag_ir").change(function(){
		if ( $(this).val() == "" )
		{
			$("#actionexecuteflag_ir_registlabel").hide();
		}
		else
		{
			$("#actionexecuteflag_ir_registlabel").show();
		}
	});

	$("#actionexecuteflag").change(function(){
		var v = $(this).val();
		if ( v == "赤外線" )
		{
			$("#actionexecuteflag_ir_div").show();
			$("#actionexecuteflag_command_div").hide();
		}
		else if ( v == "コマンド" )
		{
			$("#actionexecuteflag_ir_div").hide();
			$("#actionexecuteflag_command_div").show();
		}
		else //何もしない
		{
			$("#actionexecuteflag_ir_div").hide();
			$("#actionexecuteflag_command_div").hide();
		}
	});

	//最初は非編集モードにする.
	$("#gotoedit").change();
});
</script>

</body></html>