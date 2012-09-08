<!DOCTYPE html>
<html lang="ja"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta charset="utf-8">
	<meta name="viewport" content="width=640" />
	<title>設定</title>
	<base href="/"></base>
	<link type="text/css" href="./jscss/ui-lightness/jquery-ui-1.8.21.custom.css" rel="stylesheet" />
	<link type="text/css" href="./jscss/jquery.mobile.css" rel="stylesheet" />
	<script type="text/javascript" src="./jscss/jquery.js"></script>
	<script type="text/javascript" src="./jscss/jquery-ui.js"></script>
	<script type="text/javascript" src="./jscss/jquery.mobile.js"></script>


	<style>

	.clearfix{
		clear:both;
		float:none;
	}
	
	.error {
		display: none;
		font-color: #red;
	}

	.js_template {
		display: none;
	}
	
	.contents_wrapper {
 		width:900px;
		margin:0 auto;
	}

	dt {
		padding: 10px;
		font-size: 1.2em;
	}

	@media screen and (max-device-width: 640px), screen and (max-width: 640px) {
		.contents_wrapper {
			width:640px;
			margin:0 auto;
		}
	}
	</style>
</head>
<body class="ui-mobile-viewport ui-overlay-c"> 


<div data-role="page" id="top" data-theme="c" data-url="top" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">

	<div data-role="header"> 
		<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
	</div> <!-- /header -->

	<div data-role="content" class="ui-content contents_wrapper" role="main">


	<p><a data-role="button" class="menubutton" data-inline="true" href="/">設定</a></p>

	<p><a data-role="button" data-transition="slide" href="/remocon/" >リモコン</a></p>
	<p><a data-role="button" data-transition="slide" href="/script/" >会話シナリオ</a></p>
	<p><a data-role="button" data-transition="slide" href="/setting/" >詳細設定</a></p>

	</div><!-- /content -->

	<div data-role="footer" data-position="fixed">
		<h4>(C) 2012 rti Institute of Technology</h4> 
	</div> <!-- /footer -->

</div><!-- /page top -->


<div data-role="page" id="setting_account" data-theme="c" data-url="setting_account" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">

<div data-role="header"> 
	<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
</div> 

	<div data-role="content" data-theme="c" class="ui-content contents_wrapper">	
	<p><a data-role="button" class="menubutton" data-inline="true">設定→</a><a data-role="button" class="menubutton" data-inline="true">機材設定→</a><a data-role="button" class="menubutton" data-inline="true">アカウント設定</a></p>

	<dl>
	<dt>メールアドレス</dt>
	<dd>
		<input type="text" id="setting_account_usermail" value="">
	</dd>
	<dt>パスワード</dt>
	<dd>
		<input type="text" id="setting_account_password" value="">
	</dd>
	</dl>
	<div class="error" id="auth_error">認証に失敗しました。</div>
	<div class="error" id="auth_ok">まだ認証されていません。</div>
	<div class="error" id="auth_ok">正しく認証されています。</div>

	クラウドの処理を使うには、ユーザ登録が必要です。<br />
	<p><a data-role="button" onclick="OnActionEditablePageOpen($('#editable_key').val(),'new');" >認証する</a></p>
	<p><a data-role="button" onclick="OnActionEditablePageOpen($('#editable_key').val(),'new');" >リンクを解除する</a></p>

	</div><!-- /content -->

<div data-role="footer" data-position="fixed">
	<h4>(C) 2012 rti Institute of Technology</h4> 
</div> 

</div><!-- /page setting_account -->

<div data-role="page" id="setting_network" data-theme="c" data-url="setting_network" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">

<div data-role="header"> 
	<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<p><a data-role="button" class="menubutton" data-inline="true">設定→</a><a data-role="button" class="menubutton" data-inline="true">機材設定→</a><a data-role="button" class="menubutton" data-inline="true">ネットワーク設定</a></p>

	<dl>
	<dt>IPアドレス取得方法</dt>
	<dd>
		<select id="setting_network_ipaddress_type" size="1">
		<option value="DHCP">DHCP</option>
		<option value="固定IP">固定IP</option>
		</select>
		<div id="ipaddress_type_fixed_div">
			<dl>
			<dt>IPアドレス</dt>
			<dd>
				<input type="text" id="setting_network_ipaddress_type_fixed_ip" value="">
			</dd>
			<dt>netmask</dt>
			<dd>
				<input type="text" id="setting_network_ipaddress_type_fixed_mask" value="">
			</dd>
			<dt>gateway</dt>
			<dd>
				<input type="text" id="setting_network_ipaddress_type_fixed_gateway" value="">
			</dd>
			<dt>DNS</dt>
			<dd>
				<input type="text" id="setting_network_ipaddress_type_fixed_dns" value="">
			</dd>
			</dl>
		</div>
	</dd>
	</dl>
	<p><a data-role="button" onclick="OnActionEditablePageOpen($('#editable_key').val(),'new');" >設定する</a></p>
	</div><!-- /content -->

<div data-role="footer" data-position="fixed">
	<h4>(C) 2012 rti Institute of Technology</h4> 
</div> 

</div><!-- /page setting_network -->


<div data-role="page" id="setting_recong" data-theme="c" data-url="setting_recong" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">

<div data-role="header"> 
	<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<p><a data-role="button" class="menubutton" data-inline="true">設定→</a><a data-role="button" class="menubutton" data-inline="true">機材設定→</a><a data-role="button" class="menubutton" data-inline="true">音声認識設定</a></p>

	<dl>
	<dt>誤認識フィルタ</dt>
	<dd>
		<select id="setting_recong_recong_type" size="1">
		<option value="クラウドで再検証する">クラウドで再検証する</option>
		<option value="クライアントでの2PATHチェック">クライアントでの2PATHチェック</option>
		<option value="音声認識を利用しない">音声認識を利用しない</option>
		</select>
	</dd>
	<dt>マイク音量</dt>
	<dd>
		<label for="setting_recong_recong_volume">音量:</label>
		<input type="range" id="setting_recong_recong_volume" value="60" min="0" max="100" data-highlight="true" />
	</dd>
	<dt>呼びかけワード</dt>
	<dd>
		<input type="text" id="setting_recong_reco__yobikake__1" value="">
		<input type="text" id="setting_recong_reco__yobikake__2" value="">
		<input type="text" id="setting_recong_reco__yobikake__3" value="">
		<input type="text" id="setting_recong_reco__yobikake__4" value="">
		<input type="text" id="setting_recong_reco__yobikake__5" value="">
		<p>短すぎる呼びかけは誤動作を引き起こします。最低3文字はいれてください。</p>
	</dd>
	</dl>
	<p><a data-role="button" onclick="OnActionEditablePageOpen($('#editable_key').val(),'new');" >設定する</a></p>
	</div><!-- /content -->

<div data-role="footer" data-position="fixed">
	<h4>(C) 2012 rti Institute of Technology</h4> 
</div> 
	
</div><!-- /page setting_recong -->


<div data-role="page" id="setting_speak" data-theme="c" data-url="setting_speak" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">

<div data-role="header"> 
	<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<p><a data-role="button" class="menubutton" data-inline="true">設定→</a><a data-role="button" class="menubutton" data-inline="true">機材設定→</a><a data-role="button" class="menubutton" data-inline="true">合成音声設定</a></p>

	<dl>
	<dt>合成音声</dt>
	<dd>
		<select id="setting_speak_speak_type" size="1">
		<option value="mssp">mssp</option>
		<option value="google translate">google translate</option>
		<option value="openjtalk">openjtalk</option>
		<option value="aquestalk">aquestalk</option>
		<option value="読み上げを行わない">読み上げを行わない</option>
		</select>
	</dd>
	<dt>スピーカ音量</dt>
	<dd>
		<label for="setting_speak_speak_volume">音量:</label>
		<input type="range" id="setting_speak_speak_volume" value="60" min="0" max="100" data-highlight="true" />
	</dd>
	</dl>
	<p><a data-role="button" onclick="OnActionEditablePageOpen($('#editable_key').val(),'new');" >設定する</a></p>
	</div><!-- /content -->

<div data-role="footer" data-position="fixed">
	<h4>(C) 2012 rti Institute of Technology</h4> 
</div> 

</div><!-- /page setting_speak -->


<div data-role="page" id="setting_version" data-theme="c" data-url="setting_version" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">

<div data-role="header"> 
	<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<p><a data-role="button" class="menubutton" data-inline="true">設定→</a><a data-role="button" class="menubutton" data-inline="true">機材設定→</a><a data-role="button" class="menubutton" data-inline="true">合成音声設定</a></p>

	<dl>
	<dt>バージョン情報</dt>
	<dd>
		<span id="version">version</span><br>
		<div id="new_version">新しいバージョンがあります。<br />アップデートしますか？</div>
	</dd>
	<dt>著作表示</dt>
	<dd>
		rti giken.<br>
		<br>
		以下のソフトウェアを含んでいます<br>
		julius<br>
		openjtalk<br>
		mecab<br>
		lua<br>
		liblinear<br>
		mmdagent<br>
	</dd>
	</dl>
	<p><a data-role="button" onclick="OnActionEditablePageOpen($('#editable_key').val(),'new');" >再起動する</a></p>
	<p><a data-role="button" onclick="OnActionEditablePageOpen($('#editable_key').val(),'new');" >シャットダウンする</a></p>
	</div><!-- /content -->

<div data-role="footer" data-position="fixed">
	<h4>(C) 2012 rti Institute of Technology</h4> 
</div> 

</div><!-- /page setting_version -->

<script>

</script>

</body></html>