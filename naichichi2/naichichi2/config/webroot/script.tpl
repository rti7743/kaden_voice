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
	<p><a data-role="button" class="menubutton" data-inline="true" href="#top">設定→</a><a data-role="button" class="menubutton" data-inline="true" href="#top" >スクリプト</a></p>

	<p><a data-role="button" data-transition="slide" href="#setting_scenario" >シナリオ設定</a></p>
	<p><a data-role="button" data-transition="slide" href="#setting_command" >コマンド設定</a></p>
	<p><a data-role="button" data-transition="slide" href="#setting_webapi" >web API</a></p>

	</div><!-- /content -->

	<div data-role="footer" data-position="fixed">
		<h4>(C) 2012 rti Institute of Technology</h4> 
	</div> <!-- /footer -->
</div><!-- /page top -->


<div data-role="page" id="scriptselect" data-url="scriptselect" tabindex="0" data-theme="c" class="ui-page ui-body-c ui-page-active">
	<div data-role="header">
		<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
	</div> 

	<div data-role="content" data-theme="c" class="ui-content contents_wrapper" role="main">
	<p><a data-role="button" class="menubutton" data-inline="true" href="#top">設定→</a><a data-role="button" class="menubutton" data-inline="true" href="#top" >スクリプト</a><a data-role="button" class="menubutton" data-inline="true" href="#top" >選択</a></p>

		<div class="controll">
			<input type="combo" class="typepath" value="">
			<div style="float: left; width: 50%">
				検索:<input type="text" data-mini="true" class="search" value="">
			</div>
			<div style="float: left;">
			</div>
			<div style="float: left;" data-role="controlgroup" data-type="horizontal">
				<input type="button" value="新規作成">
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
								<a href="javascript:void(0)" data-role="button" data-icon="check"  class="icon useicon"><span>編集する</span></a>
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
</div><!-- /page scriptselect -->


<div data-role="page" id="scripteditor" data-url="scripteditor" tabindex="0" data-theme="c" class="ui-page ui-body-c ui-page-active">
	<div data-role="header">
		<h1><a data-role="none" href="#top" ><img src="./jscss/images/fhc700.png" /></h1> 
	</div> 

	<div data-role="content" data-theme="c" class="ui-content contents_wrapper" role="main">
	<p><a data-role="button" class="menubutton" data-inline="true" href="#top">設定→</a><a data-role="button" class="menubutton" data-inline="true" href="#top" >スクリプト</a><a data-role="button" class="menubutton" data-inline="true" href="#top" >選択</a><a data-role="button" class="menubutton" data-inline="true" href="#top" >編集</a></p>

		<div class="controll">
			<input type="combo" class="typepath" value="">
			<div style="float: left; width: 50%">
				検索:<input type="text" data-mini="true" class="search" value="">
			</div>
			<div style="float: left;">
			</div>
			<div style="float: left;" data-role="controlgroup" data-type="horizontal">
				<input type="button" value="セーブして実行">
				<a href="javascript:history.back()" data-role="button" class="back"><span>戻る</span></a>
			</div>
			<br class="clearfix" />
		</div><!-- /controll -->
		<textarea>
			
		</textarea>

	</div><!-- /content -->

	<div data-role="footer" data-position="fixed">
		<h4>(C) 2012 rti Institute of Technology</h4> 
	</div> <!-- /footer -->
</div><!-- /page scripteditor -->


</body></html>