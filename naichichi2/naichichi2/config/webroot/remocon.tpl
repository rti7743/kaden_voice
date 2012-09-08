<!DOCTYPE html>
<html lang="ja"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta charset="utf-8">
	<meta name="viewport" content="width=640" />
	<title>リモコン</title>
	<base href="/"></base>
	<link type="text/css" href="./jscss/jquery.mobile.css" rel="stylesheet" />
	<script type="text/javascript" src="./jscss/jquery.js"></script>
	<script type="text/javascript" src="./jscss/jquery-ui.js"></script>
	<script type="text/javascript" src="./jscss/jquery.mobile.js"></script>
	<script type="text/javaScript" src="./jscss/jquery.tooltip2.js"></script>
	<script type="text/javascript" src="./jscss/jquery.upload-1.0.2.js"></script>

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
	#remoconmenu_ul .type {
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



	#actionmenu_ul {
		list-style-type: none;
		margin: 10;
		padding: 10; 
	}
	#actionmenu_ul li {
		margin: 15px 15px 15px 15px;
		padding: 0px;
		width: 300px;
		height: 40px;
		border: 1px solid #CCCCCC;
		color: #1C94C4;
   	}

	#actionmenu_ul .itembox {
		position:	relative;
	}

	#actionmenu_ul .delicon {
		z-index:	99;
		font-size:	10px;
		position:	absolute;
		top:		0px;
		left:		0px;
	}
	#actionmenu_ul .editicon {
		z-index:	99;
		font-size:	10px;
		position:	absolute;
		top:		0px;
		left:		220px;
	}
	#actionmenu_ul .type {
		z-index:	10;
		font-size:	20px;
		position:	absolute;
		top:		5px;
		left:		40px;
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
	<p><a data-role="button" class="menubutton" data-inline="true" href="#top">設定→</a><a data-role="button" class="menubutton" data-inline="true" href="#top" >リモコン</a></p>

	<ul id="remoconmenu_li_sample" class="js_template">
		<li class="ui-state-default item">
			<div class="itembox">
				<span class="key" style="display: none;">%KEY%</span>
				<a href="javascript:void(0)" data-role="button" data-icon="delete" class="icon delicon" data-iconpos="notext"></a>
				<div class="mainicon"><img src="%ICON%" width="128" height="128" /></div>
				<div class="type">%TYPE%</div>
				<div class="status">%STATUS%</div>
				<a href="javascript:void(0)" data-role="button" data-icon="false" class="icon editicon"><span>edit</span></a>
				<div class="popupselect" style="display:none;">
					%POPUPSELECT%
				</div>
			</div>
		</li>
	</ul>
	<ul id="remoconmenu_popup_sample" class="js_template">
					<a href="javascript:void(0)">
						<span class="actionkey" style="display: none;">%ACTIONKEY%</span>
						<span class="actiontype">%ACTIONTYPE%</span>
						<br />
					</a>
	</ul>
	<ul id="remoconmenu_ul" class="ui-sortable">
	</ul>
	<br class="clearfix">

	<p><a class="editable" data-role="button" onclick="OnEditbalePageOpen('new');" >新規作成する</a></p>

	<div data-role="fieldcontain">
		<fieldset data-role="controlgroup">
			<input type="checkbox" id="gotoedit" value="1" />
			<label for="gotoedit">編集する</label>
		</fieldset>
	</div>

	</div><!-- /content -->

	<div data-role="footer" data-position="fixed">
		<h4>(C) 2012 rti Institute of Technology</h4> 
	</div> <!-- /footer -->
</div><!-- /page top -->


<div data-role="dialog" data-url="updatewaitdialog" id="updatewaitdialog" data-external-page="true" tabindex="0" class="ui-page ui-body-c ui-dialog ui-overlay-c" style="min-height: 619px; ">
	<div role="dialog" class="ui-dialog-contain ui-corner-all ui-overlay-shadow">
		<div data-role="content" data-theme="c" class="ui-corner-bottom ui-content ui-body-c" role="main">	
			<span><img src="/jscss/images/ajax-loader.gif" /></span>
			設定中・・・・
		</div><!-- /content -->
	</div><!-- /dialog-->
</div><!-- /recongresetdialog -->


<div data-role="dialog" data-url="ir_learndialog" id="ir_learndialog" data-external-page="true" tabindex="0" class="ui-page ui-body-c ui-dialog ui-overlay-c" style="min-height: 619px; ">
	<div role="dialog" class="ui-dialog-contain ui-corner-all ui-overlay-shadow">
		<div data-role="header" data-theme="d" data-position="inline" class="ui-corner-top ui-header ui-bar-d" role="banner"><a href="http://dev.screw-axis.com/jqm/ref.php?id=dialog#" data-icon="delete" data-iconpos="notext" class="ui-btn-left ui-btn ui-btn-up-d ui-shadow ui-btn-corner-all ui-btn-icon-notext" data-corners="true" data-shadow="true" data-iconshadow="true" data-wrapperels="span" data-theme="d" title="Close"><span class="ui-btn-inner ui-btn-corner-all"><span class="ui-btn-text">Close</span><span class="ui-icon ui-icon-delete ui-icon-shadow">&nbsp;</span></span></a>
			<h1 class="ui-title" role="heading" aria-level="1">赤外線学習待機中・・・・</h1>
		</div><!-- /header -->

		<div data-role="content" data-theme="c" class="ui-corner-bottom ui-content ui-body-c" role="main">	
			赤外線信号を本体に向け送信してください。<br />
		</div><!-- /content -->
	</div><!-- /dialog-->
</div><!-- /ir_learndialog -->

<div data-role="page" data-url="fileselectpage" id="fileselectpage" data-theme="c" data-url="]" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">
	<div data-role="header"> 
		<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" ></a></h1> 
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">
		<div class="drop_banner" style="display: none; position:absolute; width: 100%; height: 100%;">dropでアップロードできます</div>
		<div class="controll">
			<div style="float: left; width: 50%">
				検索:<input type="text" data-mini="true" class="search" value="">
				<input type="hidden" class="typepath" value="">
			</div>
			<div style="float: left;">
			</div>
			<div style="float: left;" data-role="controlgroup" data-type="horizontal">
				<input type="file" name="uploadfile1" value="アップロード">
				<a href="javascript:history.back()" data-role="button" class="back"><span>戻る</span></a>
			</div>
			<br class="clearfix" />
		</div><!-- /controll -->
		<ul class="filelist_li_sample js_template">
			<li>
				<span class="key" style="display: none;">%KEY%</span>
				<div class="filelist_item">
					<div style="float: left; width: 64px; height: 64px:" class="fileicon">
						<img src="%ICON%" width="64" height="64" />
					</div>
					<div>
						<div class="filelist_item_1">
							<div class="filekey" style="display: none;">%KEY%</div>
							<div class="filename">%NAME%</div>
						</div><!-- filelist_item_1 -->
						<div class="filelist_item_2">
							<div style="float: left; width: 20%">
								<span class="filesize">サイズ:<span>%SIZE%</span></span>
							</div>
							<div style="float: left; width: 30%">
								<span class="filedate">日付:<span>%DATE%</span></span>
							</div>
							<div style="float: left;" data-role="controlgroup" data-type="horizontal">
								<a href="javascript:void(0)" data-role="button" data-icon="check"  class="icon useicon"><span>これを使う</span></a>
								<a href="javascript:void(0)" data-role="button" data-icon="delete" class="icon delete"><span>削除</span></a>
							</div>
						</div><!-- filelist_item_2 -->
					</div>
					<br class="clearfix" />
				</div>
			</li>
		</ul>
		<ul class="filelist_ul ui-sortable">
		</ul><!-- /filelist_ul -->
	</div><!-- /content -->

	<div data-role="footer" data-position="fixed">
		<h4>(C) 2012 rti Institute of Technology</h4> 
	</div> <!-- /footer -->
</div><!-- /page fileselectpage -->


<div data-role="page" id="editable" data-theme="c" data-url="editable" tabindex="0" class="ui-page ui-body-c" style="min-height: 562px; ">
	<div data-role="header"> 
		<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
	</div> 

	<div data-role="content" data-theme="c" class="contents_wrapper">	
	<div class="error" id="editable_error">エラーが発生しました!!</div>
	<p><a data-role="button" class="menubutton" data-inline="true" href="#top">設定→</a><a data-role="button" class="menubutton" data-inline="true" href="#top">リモコン→</a><a data-role="button" class="menubutton" data-inline="true" onclick="OnEditbalePageOpen($('#editable_key').val());">機材設定</a></p>

	<input type="hidden" id="editable_key" value="">
	<dl>
	<dt>種類</dt>
	<dd>
		<select id="type" size="1">
		<option value="エアコン">エアコン</option>
		<option value="テレビ">テレビ</option>
		<option value="照明">照明</option>
		<optgroup label="あ-">
			<option value="エアコン">エアコン</option>
		</optgroup>
		<optgroup label="か-">
			<option value="カーテン">カーテン</option>
			<option value="空気清浄機">空気清浄機</option>
			<option value="コンポーザ">コンポーザ</option>
		</optgroup>
		<optgroup label="さ-">
			<option value="照明">照明</option>
			<option value="扇風機">エアコン</option>
		</optgroup>
		<optgroup label="た-">
			<option value="テレビ">テレビ</option>
			<option value="ディスプレイ">ディスプレイ</option>
		</optgroup>
		<optgroup label="な-">
		</optgroup>
		<optgroup label="は-">
			<option value="パソコン">パソコン</option>
			<option value="プロジェクタ">プロジェクタ</option>
		</optgroup>
		<optgroup label="ま-">
		</optgroup>
		<optgroup label="や-">
		</optgroup>
		<optgroup label="ら-">
			<option value="レコーダ">レコーダ</option>
		</optgroup>
		<optgroup label="わ-">
		</optgroup>
		<option value="その他">その他</option>
		</select>

		<div id="type_other_div">
		その他種類名:<input type="text" id="type_other" value="">
		<div class="error" id="type_other_error">その他の名前を入れてください。</div>
		</div>
	</dd>
	<dt>アイコン</dt>
	<dd>
		<div class="filelist_item">
			<div style="float: left; width: 64px; height: 64px:" class="fileicon">
				<img src="/user/elecicon/icon_elec.png" width="64" height="64" />
			</div>
			<div style="float: left;">
				<div class="filelist_item_1">
					<input type="hidden" id="elecicon" value="" data-mini="true" />
					<span id="elecicon_display"></span>
				</div><!-- filelist_item_1 -->
				<div class="filelist_item_2">
					<div style="float: left;" data-role="controlgroup" data-type="horizontal">
						<a href="javascript:void(0)" id="elecicon_select_button" data-role="button" data-icon="check"  class="icon useicon"><span>ファイルを選択する</span></a>
					</div>
				</div><!-- filelist_item_2 -->
			</div>
			<br class="clearfix" />
		</div>
	</dd>
	<dt>動作</dt>
	<dd>
		<ul id="actionmenu_li_sample" class="js_template">
			<li>
				<div class="itembox">
					<a href="javascript:void(0)"  onclick="OnEditablePageActionDelete($('#editable_key').val(),'%ACTIONKEY%');" data-role="button" data-icon="delete" class="icon delicon" data-iconpos="notext"></a>

					<span style="display: none;" class="id">%ACTIONKEY%</span>
					<span class="type">%ACTIONTYPE%</span>

					<a href="javascript:void(0)" onclick="OnActionEditablePageOpen($('#editable_key').val(),'%ACTIONKEY%');" data-role="button" data-icon="false" class="icon editicon"><span>edit</span></a>

				</div>
			</li>
		</ul>
		<ul id="actionmenu_ul" class="ui-sortable">
		</ul>
		<div id="editable_new_actionmenu">
		<p><a data-role="button" onclick="OnActionEditablePageOpen($('#editable_key').val(),'new');" >動作を新規に追加する</a></p>
		</div>
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

	<p><a data-role="button" class="menubutton" data-inline="true" href="#top">設定→</a><a data-role="button" class="menubutton" data-inline="true" href="#top">リモコン→</a><a data-role="button" class="menubutton" data-inline="true" onclick="OnEditbalePageOpen($('#editable_key').val());">機材設定→</a><a data-role="button" class="menubutton" data-inline="true"  onclick="OnActionEditablePageOpen($('#actioneditable_key1').val(),$('#actioneditable_key2').val());">動作設定</a></p>

	<input type="hidden" id="actioneditable_key1" value="">
	<input type="hidden" id="actioneditable_key2" value="">
	<dl>
	<dt>種類</dt>
	<dd>
		<select id="actiontype" size="1">
		<option value="つける">つける</option>
		<option value="けす">けす</option>
		<optgroup label="大きさ">
			<option value="小">小</option>
			<option value="中">中</option>
			<option value="大">大</option>
		</optgroup>
		<optgroup label="エアコン">
			<option value="けす">けす</option>
			<option value="冷房Minimum">冷房Minimum</option>
			<option value="冷房Normal">冷房Normal</option>
			<option value="冷房MAX">冷房MAX</option>
			<option value="暖房Minimum">暖房Minimum</option>
			<option value="暖房Normal">暖房Normal</option>
			<option value="暖房MAX">暖房MAX</option>
		</optgroup>
		<optgroup label="前後・早送り">
			<option value="つぎの">つぎの</option>
			<option value="まえの">まえの</option>
			<option value="早送り">早送り</option>
			<option value="先頭に">先頭に</option>
			<option value="終端に">終端に</option>
			<option value="最初から">最初から</option>
		</optgroup>
		<option value="取り出し">取り出し</option>
		<optgroup label="テレビ">
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
		</optgroup>
		<optgroup label="音量">
			<option value="音量ミュート">音量ミュート</option>
			<option value="音量ミュート解除">音量ミュート解除</option>
		</optgroup>
		<option value="その他">その他</option>
		</select>
		<div id="actiontype_other_div">
		その他種類名:<input type="text" id="actiontype_other" value="">
		<div class="error" id="actiontype_other_error">その他の動作名を入れてください。</div>
		</div>
	</dd>
	<dt>操作方法</dt>
	<dd>
		<label><input type="checkbox" id="actionvoice">音声認識する</label>
			<div id="actionvoice_div">
			音声認識コマンド:<input type="text" id="actionvoice_command" value="">
			<div class="error" id="actionvoice_error">音声認識するコマンドを入れてください。<br />
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
			<select id="tospeak_select" size="1">
			<option value="文字列">文字列</option>
			<option value="音楽ファイル">音楽ファイル</option>
			</select>
				<div id="tospeak_string_div">
					読み上げる文字列<br />
					<input type="text" id="tospeak_string" value=""><br />
					<div class="error" id="tospeak_string_error">読み上げる文字列を入力してください。</div>
				</div>
				<div id="tospeak_mp3_div">
					読み上げる音楽ファイル (.wav // .mp3 // .ogg などが指定できます。)<br />
					
					<div class="filelist_item">
						<div style="float: left; width: 64px; height: 64px:" class="fileicon" id="tospeak_mp3_select_preveiw" >
							<img src="/jscss/images/icon_audio_noaudio.png" width="64" height="64" />
						</div>
						<div style="float: left;">
							<div class="filelist_item_1">
								<input type="hidden" id="tospeak_mp3" value="" data-mini="true" />
								<span id="tospeak_mp3_display"></span>
							</div><!-- filelist_item_1 -->
							<div class="filelist_item_2">
								<div style="float: left;" data-role="controlgroup" data-type="horizontal">
									<a href="javascript:void(0)" id="tospeak_mp3_select_button" data-role="button" data-icon="check"  class="icon useicon"><span>ファイルを選択する</span></a>
								</div>
							</div><!-- filelist_item_2 -->
						</div>
						<br class="clearfix" />
					</div>
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
				<input type="hidden" id="actionexecuteflag_ir" value=""><br />
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
				<select id="actionexecuteflag_ir_freq" size="1">
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
				<input type="text" id="actionexecuteflag_command" value="コマンドを指定する">
				<div class="error" id="actionexecuteflag_command_error">コマンドを指定してください<br />
				登録されているコマンドについては、設定→コマンド　から確認してください。<br />
				</div>
			</dd>
			<dt>引数1<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args1" value="">
			</dd>
			<dt>引数2<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args2" value="">
			</dd>
			<dt>引数3<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args3" value="">
			</dd>
			<dt>引数4<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args4" value="">
			</dd>
			<dt>引数5<dt>
			<dd>
				<input type="text" id="actionexecuteflag_command_args5" value="">
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
var g_Callback ={};

var g_editableObject = null;
var g_actioneditableObject = null;

function typeotherSelect(type,typeother)
{
	if (type == "その他") return typeother;
	return type;
}

function showUpdatewaitDialog( transition )
{
	$.mobile.changePage('#updatewaitdialog', {transition: "slidedown", role: 'dialog'}); 
}
function hideUpdatewaitDialog( transition )
{
	$("#updatewaitdialog").dialog("close");
}


function showFileSelectPage( typepath , selectfilename , message, okfunction )
{
	g_Callback["filelist_use"] = okfunction;

	var fileselectpageObject = $("#fileselectpage");
	fileselectpageObject.find(".typepath").val(typepath);
	fileselectpageObject.find(".search").keydown();

	$.mobile.changePage("#fileselectpage", { transition: "flip" });
}


//見やすいサイズに変換
function sizescale(sizeString)
{
	var i = parseInt(sizeString);
//	if (i > 1000000000000) return parseInt(i / 1000000000000) + "TByte";
	if (i > 1000000000 ) return parseInt(i / 1000000000) + "GByte";
	if (i > 1000000) return parseInt(i / 1000000) + "MByte";
	if (i > 1000) return parseInt(i / 1000) + "KByte";
	return i + "Byte";
}


//アイテムに動作を付ける.
function itemaction() {
	//ドキュメントの幅
	var documentwidthPer3 = (document.documentElement.scrollWidth || document.body.scrollWidth) /3;
	
	//既存のアイコンをすべて消す
	$("#remoconmenu_ul").html("");

	var htmltemplate = $("#remoconmenu_li_sample").html();
	var htmltemplatepopup = $("#remoconmenu_popup_sample").html();
	var totaldom = "";
	for(var arraykey in g_Remocon)
	{
		var s = arraykey.split("_");
		if ( !(s[0] == "elec" && s[2] == "type" && s[3] == undefined )) continue;
		var key = s[1];

		var html = htmltemplate;
		html = html.replace(/%KEY%/g , key);
		html = html.replace(/%ICON%/g , "/user/elecicon/" + g_Remocon["elec_" + key + "_elecicon"]);
		html = html.replace(/%TYPE%/g , g_Remocon["elec_" + key + "_type"] );
		html = html.replace(/%STATUS%/g , g_Remocon["elec_" + key + "_status"]);

		var totalhtmlpopup = "";
		for(var arraykey2 in g_Remocon)
		{
			var s = arraykey2.split("_");
			if ( !(s[0] == "elec" && s[1] == key && s[2] == "action" && s[4] == "type" && s[5] == undefined)) continue;
			var actionkey = s[3];

			var htmlpopup = htmltemplatepopup;
			htmlpopup = htmlpopup.replace(/%ACTIONKEY%/g , actionkey);
			htmlpopup = htmlpopup.replace(/%ACTIONTYPE%/g , g_Remocon["elec_" + key + "_action_" + actionkey + "_actiontype"] );
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
		var menutype = itemObject.find(".type").text();

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
			var actiontype = $(this).children(".actiontype").text();

			updatestats(itemObject,contentObject,actiontype);
			remocon_fire_bytype(menutype,actiontype);
		});

		//でかいアイコンをクリックした時に、ステータスをトグルスイッチみたいに切り替えます。
		itemObject.click(function(){
			if (g_enableEditMode) {
				return false;
			}
			var status = itemObject.find(".status").text();
			var newstatus = "";
			var hit = false;
			var actiontype = itemObject.find(".actiontype").each(function(){
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
			remocon_fire_bytype(menutype,newstatus);
			return false;
		});

		//削除アイコンがクリックされた時の動作
		itemObject.find(".delicon").click(function(){
			if ( window.confirm("削除してもよろしいですか？") )
			{
				var arr = {};
				arr["key"] = menukey;
			
				//update待ちにする
				showUpdatewaitDialog();

				//サーバにデータを送信
				//editable_key=123&type=タイプ&type_other=タイプその他&elecicon=filename
				$.ajax({
						type: "post",
						url: "/remocon/delete/elec",
						timeout: 50000,
						data: arr,
						dataType: 'json',
						success: function(d){
							if ( d['status'] != "ok" )
							{
								hideUpdatewaitDialog();
								alertErrorDialog(d,"remocon_delete_elec関数");
								return ;
							}

							//画面の情報を全入れ替え
							g_Remocon = d;
							
							//トップ画面を更新
							itemaction();
							
							//トップ画面に戻す
							$.mobile.changePage("#top", { transition: "flip" });
							return ;
						},
						error: function(d){
							hideUpdatewaitDialog();
							alertErrorDialog(d,"remocon_delete_elec関数");
							return ;
						}
				});
			}
		});

		//編集アイコンがクリックされた時の動作
		itemObject.find(".editicon").click(function(){
			OnEditbalePageOpen(menukey);
			return false;
		});

	});

	//編集モード、非編集モードの切替.
	$("#gotoedit").change();
}






//選択されたのでステータスを更新する.
function updatestats(itemObject,contentObject,newstatus)
{
	//表示するステータスを更新する.
	itemObject.find(".status").text(newstatus);

	contentObject.find(".actiontype").each(function(){
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
function remocon_fire_bytype(type1,type2)
{
	$.get("/remocon/fire/bytype?type1=" + type1 + "&type2=" + type2, function(){
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
	$.post("/remocon/update/icon_order",orderbyArray, function(d){
		if ( d['status'] != "ok" )
		{
			alertErrorDialog(d,"remocon_update_icon_order関数");
			return ;
		}
		g_Remocon = d;
	},'json');
}


//ajaxのエラー表示
function alertErrorDialog(ajaxReturn,functionName)
{
	if ( ajaxReturn['status'] == "error" )
	{
		alert(functionName + "が、失敗しました。\r\n" + "code: " + ajaxReturn['code'] + "\r\n" + "message: " + ajaxReturn['message'] );
	}
	else
	{
		alert(functionName + "が、不明な応答を返ししました。");
	}
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
	$.post("/remocon/update/elec_action_order",orderbyArray, function(d){
		if ( d['status'] != "ok" )
		{
			alertErrorDialog(d,"remocon_update_elec_action_order関数");
			return ;
		}
//		g_Remocon = d;
	},'json');
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
	updateArray["type"] = $("#type").val();
	updateArray["type_other"] = $("#type_other").val();
	if (updateArray["type"] == "")
	{
		$("#type_error").show();
		bad = true;
	}
	if (updateArray["type"] == "その他" && updateArray["type_other"] == "")
	{
		$("#type_other_error").show();
		bad = true;
	}
	updateArray["elecicon"] = $("#elecicon").val();

	if ( bad )
	{//エラー
		$("#editable_error").show();
		return false;
	}
	
	//update待ちにする
	showUpdatewaitDialog();

	//サーバにデータを送信
	//editable_key=123&type=タイプ&type_other=タイプその他&elecicon=filename
	$.ajax({
			type: "post",
			url: "/remocon/update/elec",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				if ( d['status'] != "ok" )
				{
					hideUpdatewaitDialog();
					alertErrorDialog(d,"remocon_update_elec関数");
					return ;
				}

				//画面の情報を全入れ替え
				g_Remocon = d;
				
				//トップ画面を更新
				itemaction();
				
				//トップ画面に戻す
				$.mobile.changePage("#top", { transition: "flip" });
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();
				alertErrorDialog(d,"remocon_update_elec関数");
				return ;
			}
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
	updateArray["actiontype"] = $("#actiontype").val();
	updateArray["actiontype_other"] = $("#actiontype_other").val();
	updateArray["actionvoice"] = $("#actionvoice").prop("checked") ? 1 : 0;
	updateArray["actionvoice_command"] = $("#actionvoice_command").val();
	updateArray["showremocon"] = $("#showremocon").prop("checked") ? 1 : 0;
	updateArray["useapi"] = $("#useapi").prop("checked") ? 1 : 0;
	updateArray["useinternet"] = $("#useinternet").prop("checked") ? 1 : 0;
	updateArray["tospeak"] = $("#tospeak").prop("checked") ? 1 : 0;
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

	if (updateArray["actiontype"] == "")
	{
		$("#actiontype_error").show();
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
	
	//update待ちにする
	showUpdatewaitDialog();

	//サーバにデータを送信
	//actioneditable_key1=123&actioneditable_key2=456&actiontype=種類&actiontype_other=種類その他&actionvoice=1&actionvoice_command=こんぴゅーた&showremocon=1&useapi=1&useinternet=1&tospeak_select=ターゲット&tospeak_string=読み上げ文字列&tospeak_mp3=hoge.mp3&actionexecuteflag=赤外線&actionexecuteflag_ir=12345678910&actionexecuteflag_command=a.exe&actionexecuteflag_command_args1=&actionexecuteflag_command_args2=&actionexecuteflag_command_args3=&actionexecuteflag_command_args4=&actionexecuteflag_command_args5=
	$.ajax({
			type: "post",
			url: "/remocon/update/elec_action",
			timeout: 50000,
			data: updateArray,
			dataType: 'json',
			success: function(d){
				if ( d['status'] != "ok" )
				{
					hideUpdatewaitDialog();
					alertErrorDialog(d,"remocon_update_elec_action関数");
					return ;
				}

				g_Remocon = d;

				//トップ画面を更新
				itemaction();

				//機材の画面を更新
				OnEditbalePageOpen(parentkey);

				//機材画面に戻す
				$.mobile.changePage("#editable", { transition: "flip" });
				return ;
			},
			error: function(d){
				hideUpdatewaitDialog();
				alertErrorDialog(d,"remocon_update_elec_action関数");
				return ;
			}
	});

	//データを更新
	return true;
}

//selectタグでtargetを選択させるのだが、 targetが存在していない場合は、その他 を選択する.
function optionSelect(obj,target)
{
	var found = false;
	obj.find("option").each(function(){
		if ( $(this).attr('value') == target )
		{
			found = true;
			return false;
		}
	});
	if (found)
	{
		obj.val(target);
	}
	else
	{
		obj.val("その他");
	}
	return obj;
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
		g_editableObject.find("#type").val("");
		g_editableObject.find("#type_other_div").hide();
		g_editableObject.find("#type_other").val("");
		g_editableObject.find("#elecicon").val("icon_elec.png");
		g_editableObject.find("#elecicon_display").html("icon_elec.png");
		g_editableObject.find(".fileicon img").prop("src","/user/elecicon/icon_elec.png");
		
		$("#editable_new_actionmenu").hide();
	}
	else
	{//新規作成でないので、データを取得してくる.
		if (! g_Remocon["elec_" + key + "_type"] )
		{
			//データがない!!
			alert("指定されたデータが見つかりません。削除されている可能性があります。");
			return ;
		}
		optionSelect(g_editableObject.find("#type") , g_Remocon["elec_" + key + "_type"] ).change();
		g_editableObject.find("#type_other").val(g_Remocon["elec_" + key + "_type"]);

		g_editableObject.find("#elecicon").val(g_Remocon["elec_" + key + "_elecicon"]);
		g_editableObject.find("#elecicon_display").html(g_Remocon["elec_" + key + "_elecicon"]);
		g_editableObject.find(".fileicon img").prop("src","/user/elecicon/" + g_Remocon["elec_" + key + "_elecicon"]);


		var htmltemplate = $("#actionmenu_li_sample").html();
		var totaldom = "";
		for(var arraykey2 in g_Remocon)
		{
			var s = arraykey2.split("_");
			if ( !(s[0] == "elec" && s[1] == key && s[2] == "action" && s[4] == "actiontype" && s[5] == undefined )) continue;
			var actionkey = s[3];

			var html = htmltemplate;
			html = html.replace(/%ACTIONKEY%/g,actionkey);
			html = html.replace(/%ACTIONTYPE%/g,g_Remocon["elec_" + key + "_action_" + actionkey + "_actiontype"] );
			totaldom += html;
		}
		g_editableObject.find("#actionmenu_ul").html(totaldom);
		g_editableObject.find("#editable_new_actionmenu").show();
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
	var v = parseInt(boolString);
	if (v)
	{
		 obj.prop("checked",true);
	}
	else
	{
		 obj.prop("checked",false);
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
		$("#actiontype").val("").change();	//種類

		$("#actionvoice_command").val("").change();
		BoolToChcked( $("#actionvoice") ,"1").change();	//音声認識する
		BoolToChcked( $("#showremocon") ,"1").change();	//リモコン画面に表示する
		BoolToChcked( $("#useapi") ,"1").change();			//APIからの操作を許可する
		BoolToChcked( $("#useinternet") ,"1").change();	//インターネットからの操作を許可する

		$("#tospeak_select").val("文字列").change();
		$("#tospeak_string").val("${家電名}を${操作名}します").change();
		$("#tospeak_mp3").val("").change();
		g_actioneditableObject.find("#tospeak_mp3_display").html("未設定");
		g_actioneditableObject.find(".fileicon img").prop("src","/jscss/images/icon_audio_noaudio.png");
		BoolToChcked( $("#tospeak") ,"1").change();		//実行時にしゃべる


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
		if ( getElecText("actiontype",parentkey,key) == "")
		{
			//データがない!!
			alert("指定されたデータが見つかりません。削除されている可能性があります。");
			return ;
		}

		optionSelect($("#actiontype") ,getElecText("actiontype",parentkey,key) ).change();
		$("#actiontype_other").val( getElecText("actiontype",parentkey,key) ).change();

//alert("actionvoice:" + getElecText("actionvoice",parentkey,key) + " // showremocon:" + getElecText("showremocon",parentkey,key) + " // useapi:" + getElecText("useapi",parentkey,key) + " // useinternet:" + getElecText("useinternet",parentkey,key) + " // tospeak:" + getElecText("tospeak",parentkey,key) );

		$("#actionvoice_command").val( getElecText("actionvoice_command",parentkey,key) ).change();		//音声認識のコマンド
		BoolToChcked( $("#actionvoice") , getElecText("actionvoice",parentkey,key) ).change();			//音声認識する

		BoolToChcked( $("#showremocon") ,getElecText("showremocon",parentkey,key) ).change();			//リモコン画面に表示する
		BoolToChcked( $("#useapi")      ,getElecText("useapi",parentkey,key) ).change();				//APIからの操作を許可する
		BoolToChcked( $("#useinternet") ,getElecText("useinternet",parentkey,key) ).change();			//インターネットからの操作を許可する

		$("#tospeak_select").val(getElecText("tospeak_select",parentkey,key)).change();					//読み上げ方法
		$("#tospeak_string").val(getElecText("tospeak_string",parentkey,key)).change();					//文字列読み上げ
		$("#tospeak_mp3").val(getElecText("tospeak_mp3",parentkey,key)).change();						//音楽ファイル読み上げ
		g_actioneditableObject.find("#tospeak_mp3_display").html(getElecText("tospeak_mp3",parentkey,key));
		g_actioneditableObject.find(".fileicon img").prop("src","/jscss/images/icon_audio.png");
		BoolToChcked( $("#tospeak") ,getElecText("tospeak",parentkey,key) ).change();					//実行時にしゃべる


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
	$("#actioneditable").find("input[type='checkbox']").checkboxradio("refresh");
}


function OnEditablePageActionDelete(parentkey,key)
{
	//いたずら防止のために補正する.
	parentkey = parseInt(parentkey);
	key = parseIntOrNew(key);

	if (window.confirm("削除してもよろしいですか？") )
	{
		var arr = {};
		arr["key1"] = parentkey;
		arr["key2"] = key;
	
		//update待ちにする
		showUpdatewaitDialog();

		//サーバにデータを送信
		$.ajax({
				type: "post",
				url: "/remocon/delete/elec_action",
				timeout: 50000,
				data: arr,
				dataType: 'json',
				success: function(d){
					hideUpdatewaitDialog();
					if ( d['status'] != "ok" )
					{
						alertErrorDialog(d,"remocon_delete_elec_action関数");
						return ;
					}

					//画面の情報を全入れ替え
					g_Remocon = d;
					
					$("#actionmenu_ul li").each(function(){
						var itemObject = $(this);
						var actionkey = itemObject.find(".id").text();
						if (actionkey == key)
						{
							itemObject.hide();
							return false;
						}
						return true;
					});
					return ;
				},
				error: function(d){
					hideUpdatewaitDialog();
					alertErrorDialog(d,"remocon_delete_elec_action関数");
					return ;
				}
		});
	}
}


$(function() {
	g_editableObject = $("#editable");
	g_actioneditableObject = $("#actioneditable");

	//jquery ui sorttable を構築
	$( "#remoconmenu_ul" ).sortable({
		update: function(event, ui) { remocon_update_icon_order(); }
	});
	$( "#actionmenu_ul" ).sortable({
		update: function(event, ui) { remocon_update_elec_action_order(); }
	});

	//編集モード
	$("#gotoedit").change(function(){
		if ( $(this).prop("checked") ) {
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

	$("#elecicon_select_button").click(function(){
		showFileSelectPage( "elecicon" , $("#elecicon").val() , "アイコンを選択してください"
		, function(typename,filename){
			//ファイル名を確定
			g_editableObject.find("#elecicon").val(filename);
			g_editableObject.find("#elecicon_display").html(filename);
			
			//画像をセットする
			g_editableObject.find(".fileicon img").prop("src","/user/elecicon/" + filename);
		});
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
		if ( $(this).prop("checked") )
		{
			$("#actionvoice_div").show();
		}
		else
		{
			$("#actionvoice_div").hide();
		}
	});

	$("#tospeak").change(function(){
		if ( $(this).prop("checked") )
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
	
	$("#actionexecuteflag_ir_leaning").click(function(){
		$.mobile.changePage('#ir_learndialog', {transition: 'slidedown', role: 'dialog'});

		var arr = { };
		$.ajax({
				type: "post",
				url: "/remocon/ir_learndialog/learn",
				timeout: 50000,
				data: arr,
				dataType: 'json',
				success: function(d){
					if ( d['status'] != "ok" )
					{
						$("#ir_learndialog").find(".message").html("失敗しました");
						$("#ir_learndialog").delay(1500).dialog("close");
						return ;
					}

					$("#ir_learndialog").dialog("close");
					return ;
				},
				error: function(e){
					$("#ir_learndialog").find(".message").html("失敗しました");
					$("#ir_learndialog").delay(15000).dialog("close");
					return ;
				}
		});
	});

	$("#tospeak_mp3_select_button").click(function(){
		showFileSelectPage( "tospeak_mp3" , $("#tospeak_mp3").val() , "読み上げるファイルを選択してください"
		, function(typename,filename){

			//ファイル名を確定
			g_actioneditableObject.find("#tospeak_mp3").val(filename);
			g_actioneditableObject.find("#tospeak_mp3_display").html(filename);
			
			//画像をセットする
			g_actioneditableObject.find(".fileicon img").prop("src","/jscss/images/icon_audio.png");
		});
	});

	//読み上げるtextをプレビュー再生
	$("#tospeak_text_select_preveiw").click(function(){
		var arr = {};
		arr["tospeak_text"] = $("#tospeak_text").val();

		$.post("/remocon/preview/tospeak_text",arr, function(d){
				if ( d['status'] != "ok" )
				{
					alertErrorDialog(d,"remocon_preview_tospeak_text関数");
					return ;
				}
		},'json');
	});

	//読み上げるmp3をプレビュー再生
	$("#tospeak_mp3_select_preveiw").click(function(){
		var arr = {};
		arr["tospeak_mp3"] = $("#tospeak_mp3").val();

		$.post("/remocon/preview/elec_mp3",arr, function(d){
				if ( d['status'] != "ok" )
				{
					alertErrorDialog(d,"remocon_preview_elec_mp3関数");
					return ;
				}
		},'json');
	});


	//ファイル選択ダイアログ
	var fileselectpageObject = $("#fileselectpage");
	var fileselectpageSearchObject = fileselectpageObject.find(".search");

	//検索したときにリアルタイムに書き換える
	fileselectpageSearchObject.keydown(function(){
		fileselectpageObject.find(".filelist_ul").html("検索中・・・・");

		var arr = { };
		arr["typepath"] = fileselectpageObject.find(".typepath").val();
		arr["search"] = $(this).val();
		$.post("/remocon/fileselectpage/find",arr, function(d){
			if ( d['status'] != "ok" )
			{
				alertErrorDialog(d,"remocon_fileselectpage関数");
				return ;
			}

			var totaldom = "";
			var htmltemplate = fileselectpageObject.find(".filelist_li_sample").html();
			for(var arraykey in d)
			{
				var s = arraykey.split("_");
				if ( !(s[0] == "file" && s[2] == "name" && s[3] == undefined )) continue;
				var key = s[1];

				var html = htmltemplate;
				html = html.replace(/%KEY%/g , key);
				html = html.replace(/%ICON%/g , d["file_" + key + "_icon"] );
				html = html.replace(/%NAME%/g , d["file_" + key + "_name"] );
				html = html.replace(/%SIZE%/g , sizescale(d["file_" + key + "_size"]) );
				html = html.replace(/%DATE%/g , d["file_" + key + "_date"]);

				totaldom += html;
			}
			fileselectpageObject.find(".filelist_ul").html(totaldom);

			//イベントを設定する.
			fileselectpageObject.find(".filelist_ul li").each(function(){
				var li = $(this);
				var key = li.find(".filekey").text();
				li.find(".delete").click(function(){
					if (window.confirm("削除してもよろしいですか？") )
					{
						alert("削除");
					}
					return false;
				});
				li.find(".useicon").click(function(){
					history.back();
					g_Callback["filelist_use"](arr["typepath"] , li.find(".filename").html());
				});
				li.find(".previewicon").click(function(){
				});
			});
		},'json');
	});

	//ajax upload の準備
	fileselectpageObject.find('input[type=file]').change(function() {
		var typepath = fileselectpageObject.find(".typepath").val();
		$(this).upload('/remocon/fileselectpage/upload?typepath=' + typepath, function(res) {


		}, 'json');
	});

	//なんかうまくいかない
	// ドラッグドロップからの入力
	fileselectpageObject.bind("drop", function (e) {
		//http://www.appelsiini.net/2009/10/html5-drag-and-drop-multiple-file-upload
		var data = e.dataTransfer;

		var boundary = '------multipartformboundary' + (new Date).getTime();
		var dashdash = '--';
		var crlf     = '\r\n';

		/* Build RFC2388 string. */
		var builder = '';

		builder += dashdash;
		builder += boundary;
		builder += crlf;

		var xhr = new XMLHttpRequest();

		/* For each dropped file. */
		for (var i = 0; i < data.files.length; i++) {
			var file = data.files[i];

			/* Generate headers. */            
			builder += 'Content-Disposition: form-data; name="uploadfile' + (i+1) + '"';
			if (file.fileName) {
				builder += '; filename="' + file.fileName + '"';
			}
			builder += crlf;

			builder += 'Content-Type: application/octet-stream';
			builder += crlf;
			builder += crlf; 

			/* Append binary data. */
			builder += file.getAsBinary();
			builder += crlf;

			/* Write boundary. */
			builder += dashdash;
			builder += boundary;
			builder += crlf;
		}

		/* Mark end of the request. */
		builder += dashdash;
		builder += boundary;
		builder += dashdash;
		builder += crlf;
				alert(builder);

		var typepath = fileselectpageObject.find(".typepath").val();
		xhr.open("POST", "/remocon/fileselectpage/upload?typepath=" + typepath, true);
		xhr.setRequestHeader('content-type', 'multipart/form-data; boundary=' + boundary);
		xhr.sendAsBinary(builder);

		xhr.onload = function(e) { 
			/* If we got an error display it. */
			if (xhr.responseText) {
				alert(xhr.responseText);
			}
			fileselectpageSearchObject.keydown();
		};
		e.stopPropagation();
	})
	.bind("dragenter", function () {
		// false を返してデフォルトの処理を実行しないようにする
		return false;
	})
	.bind("dragover", function () {
		// false を返してデフォルトの処理を実行しないようにする
		return false;
	});

	//家電アイコンに動作を付ける.
	itemaction();

});
</script>

</body></html>