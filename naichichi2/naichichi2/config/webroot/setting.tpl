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
<div>
	<div class="menu"  style="float: left;">
		コンテンツ
		<ul type="disc">
		<li><a href="javascript:void(0)" onclick='loadPage("state")'>ステータス</a></li>
		<li><a href="javascript:void(0)" onclick='loadPage("network")'>ネットワーク</a></li>
			<ul type="disc">
			<li><a href="javascript:void(0)" onclick='loadPage("network")'>ネットワーク全般</a></li>
			<li><a href="javascript:void(0)" onclick='loadPage("networkmime")'>MIME</a></li>
			</ul>
		<li><a href="javascript:void(0)" onclick='loadPage("media")'>メディア</a></li>
			<ul type="disc">
			<li><a href="javascript:void(0)" onclick='loadPage("media")'>メディア全般</a></li>
			<li><a href="javascript:void(0)" onclick='loadPage("mediaext")'>拡張子</a></li>
			</ul>
		<li><a href="javascript:void(0)" onclick='loadPage("reco")'>音声認識</a></li>
		<li><a href="javascript:void(0)" onclick='loadPage("speak")'>音声合成</a></li>
		<li><a href="javascript:void(0)" onclick='loadPage("action")'>アクション</a></li>
			<ul type="disc">
			<li><a href="javascript:void(0)" onclick='loadPage("action")'>アクション全般</a></li>
			<li><a href="javascript:void(0)" onclick='loadPage("actionsetting")'>アクション設定</a></li>
			</ul>
		<li><a href="javascript:void(0)" onclick='loadPage("passion")'>感情</a></li>
		</ul>
	</div>
	<div class="contents" id="contents">
		<div class="menu_state">
			<h1>ステータス</h1>
			<div class="key">version</div><div class="value"><?= out["version"] ?></div>
		</div>
		<div class="menu_network">
			<h1>ネットワーク</h1>
			<div class="key">本体IPアドレス</div><div class="value">PC版は設定できません</div>
			<br />
			<br />
			<div class="key">web root</div><div class="value"><input type="text" value='<?= out["httpd__webroot"] ?>' name="httpd__webroot"></div>
			<br />
			<br />
			<div class="key">web port</div><div class="value"><input type="text" value='<?= out["httpd__port"] ?>' name="httpd__port"></div>
			<br />
			<br />
			<div class="key">web workerthread</div><div class="value"><input type="text" value='<?= out["httpd__workerthread"] ?>' name="httpd__port"></div>
			<br />
			<br />
		</div>
		<div class="menu_MIME">
			<h1>ネットワーク MIME</h1>
			拡張子のMIMEを設定します。<br />
			MIMEが定義されていないファイルへはアクセスが出来ません。<br />
			<br/>
			<table id="table_httpd__allowext">
				<?lua for(key,value in pairs(out["httpd__allowext"])){ ?>
				<tr>
					<td></td><td><input type="text" value="<?= key ?>" name="httpd__allowext_<?= value ?>"></td>
				</tr>
				<?lua } ?>
				<tr>
					<td><td><td><input type="Add" onclick=""><td>
				</tr>
			</table>
			web root<br/>
			web port<br/>
			web workerthread<br/>
		</div>
		<div class="menu_media">
			<h1>ネットワーク MIME</h1>
			拡張子のMIMEを設定します。<br />
			MIMEが定義されていないファイルへはアクセスが出来ません。<br />
			<br/>
			<table id="table_httpd__allowext">
				<?lua for(key,value in pairs(out["httpd__allowext"])){ ?>
				<tr>
					<td></td><td><input type="text" value="<?= key ?>" name="httpd__allowext_<?= value ?>"></td>
				</tr>
				<?lua } ?>
				<tr>
					<td><td><td><input type="Add" onclick=""><td>
				</tr>
			</table>
			web root<br/>
			web port<br/>
			web workerthread<br/>
		</div>
	</div>
</div>
<div class="clearfix"></div><br />

</body>
<script type="text/javaScript" src="./jscss/jquery.js"></script>
<script type="text/javaScript" src="./jscss/jquery.tooltip2.js"></script>
<script type="text/javaScript">


//メイン
$(function(){

});
</script>
</html>