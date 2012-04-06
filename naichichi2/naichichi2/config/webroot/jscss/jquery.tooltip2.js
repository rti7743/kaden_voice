/*
 * jQuery UI Tooltip2 @VERSION
 *
 * hacked by rti
 *
 * without jquery ui.
 */
(function($) {

	var increments = 0;
	jQuery.fn.tooltip2 = function(method,option,option2) {

		var name = '__instance_tooltip2';
		var sself = this;

		if (! sself.data(name) )
		{
			var obj =
				{
					options: {
						//表示するコンテンツ.
						//既存のコンテンツを表示したい場合は content: $('hogehoge')
						//文字列を渡したい場合は content: '文字列'
						 content: function() {
							return this.attr("tooltip");
						}

						//表示位置
						,position: {
			//				 my: "left top"
			//				,at: "left bottom"
							 my: "auto"
							,at: "auto"
							,collision: "fitp"
						}

						//追加するcssのクラス名
						,className:		''

						//直接色とかを弄りたい場合はこことかに書けば?
						,css: { }

						//やじるしの種類.
						//arrow					普通の正三角形の矢印
						//arrow-half1			二等辺三角形の矢印1
						//arrow-half2			二等辺三角形の矢印2
						//border-arrow			borderでかこっていても変にならない矢印
						//border-arrow-half1	borderでかこっていても変にならない二等辺三角形の矢印1
						//border-arrow-half2	borderでかこっていても変にならない二等辺三角形の矢印2
						//mark					ハートマークとか、任意マークの吹出し
						//bubble				思っているみたいな証言
						//none					矢印を描画しない
						,arrowtype:		'arrow'

						//吹出しの方向.
						//top left			A 上のラインの左側		    A         B         C
						//top middle		B 上のラインの真ん中	 --------------------------
						//top right			C 上のラインの右側		G|                        |J
						//bottom left		D 下のラインの左側		 |                        |
						//bottom middle		E 下のラインの真ん中	 |                        |
						//bottom right		F 下のラインの右側		H|                        |K
						//left top			G 左のラインの上側		 |                        |
						//left middle		H 左のラインの真ん中	 |                        |
						//left bottom		I 左のラインの下側		 |                        |
						//right left		J 右のラインの上側		I|                        |L
						//right middle		K 右のラインの真ん中	 --------------------------
						//right bottom		L 右のラインの下側			D         E         F
						,arrowstyle:	'top left'

						//矢印の大きさ
						//入力しないと勝手に推測する.
						,arrowsize:		null

						//やじるしに表示する文字列 arrowtype == 'mark' のときのみ.
						//おすすめは   ハート    &hearts;
						//             丸        &#9679;
						,arrowcontent:	'&hearts;'
						
						//噴出しの位置をこれだけ縦横に移動する
						,arrowoffset:	{
							 x:	0
							,y:	0
						}

						//吹き出しを表示するときのエフェクト.
						//show: function(){} で自由な処理をかける.
						//show: 'name' で、show に指定できる表示形式が書ける.
						,show: function() {
							this.fadeIn();
						}

						//吹き出しを消す時のエフェクト
						//hide: function(){} で自由な処理をかける.
						//hide: 'name' で、show に指定できる表示形式が書ける.
						,hide: function() {
							this.fadeOut();
						}

						//吹き出しを表示する条件
						//mouseover		マウスが領域に乗っかるとイベントがでる
						//click			クリックすると吹き出しが出る
						//none			何もしない
						,showoption: 'mouseover'

						//メニューを隠すイベントが発動するまでの遅延時間(単位: ミリセカンド)
						//これがあるから、ツールチップにフォーカスを移動できるんだぜ.
						//参考: http://jqueryfordesigners.com/coda-popup-bubbles/
						,hideDelayTime: 500
						
						//有効無効フラグ
						,disabled: false
					},
					_arrowStyleSetting: function() {
						var self = this;
						//こんなふうに思っているみたいな心の動きのマーク
						var _setMarkStyle = function()
						{
							var halftype = _getArrowHalfType();
							var arrowsize = _getArrowSize();
							var arrowbordersize = _getArrowBorderSize();
							var arrowstyle = _getArrowStyle();

							var top = 0,left = 0;		//四隅の幅,
							var top2 = 0,left2 = 0;		//四隅の幅,

							//この数字だけ引き釣り下ろして陥没させる
							var downoffset = self.options.arrowtype == 'mark' ? 0 : arrowsize/2;

							//マークを表示する.
							if (arrowstyle[0] == 'top' || arrowstyle[0] == 'bottom') {
								if (arrowstyle[0] == 'top') {
									top = (-arrowsize*2)+downoffset*3-arrowbordersize;
									top2 = top - arrowsize/4;
								} else {
									top = self.tooltip.height() - downoffset + arrowbordersize;
									top2 = top + arrowsize/2;
								}

								if (arrowstyle[1] == 'left') {
									left = -arrowbordersize;
									left2 = -arrowbordersize + arrowsize;
								} else if (arrowstyle[1] == 'middle') {
									left = Math.floor((self.tooltip.width() - arrowsize) / 2)-arrowbordersize;
									left2 = left + arrowsize;
								} else { //right
									left = self.tooltip.width() - arrowsize - arrowbordersize;
									left2 = left - arrowsize/2;
								}
							} else { //left or right
								if (arrowstyle[0] == 'left') {
									left = (-arrowsize)+downoffset-arrowbordersize;
									left2 = left -arrowsize/4
								} else {
									left = self.tooltip.width() + arrowbordersize - downoffset;
									left2 = left +arrowsize/2;
								}

								if (arrowstyle[1] == 'top') {
									top = -arrowbordersize;
									top2 = top + arrowsize;
								} else if (arrowstyle[1] == 'middle') {
									top = Math.floor((self.tooltip.height() - arrowsize) / 2)-arrowbordersize;
									top2 = top + arrowsize;
								} else { //bottom
									top = self.tooltip.height() - arrowsize - arrowbordersize;
									top2 = top - arrowsize/2;
								}
							}

							var arrowcolor = self.tooltip.css('background-color');
							var arrowbordercolor = _getArrowColor();

							self.arrow.css({
								 'position': 'absolute'
								,'top': top + self.options.arrowoffset.y + 'px'
								,'left': left + self.options.arrowoffset.x + 'px'
							});
							self.arrow2.css({
								 'position': 'absolute'
								,'top': top2 + self.options.arrowoffset.y + 'px'
								,'left': left2 + self.options.arrowoffset.x + 'px'
								,'zIndex': self.arrow.css('zIndex') + 1 //手前に
							});
							
							if (self.options.arrowtype == 'mark'){
								//マークを描画
								self.arrow.css({
									 'color': arrowbordercolor
									,'font-size': arrowsize*1.5  + "pt"
								}).html(self.options.arrowcontent);
								self.arrow2.css({
									 'color': arrowbordercolor
									,'font-size': arrowsize*1.0 + "pt"
								}).html(self.options.arrowcontent);
							} else {
								//思っている系のこころの動きを表示.

								var circlesize = arrowsize - arrowbordersize;
								var circlesize2 = parseInt(circlesize/2);

								if ($.browser.msie && $.browser.version < 9) {
									//我らがIE様の場合 VML でエミュレーションする.
									if (!document.namespaces.v) { 
										document.namespaces.add('v', 'urn:schemas-microsoft-com:vml');
										document.namespaces.add('o', 'urn:schemas-microsoft-com:office:office');
									}
									self.arrow.html('<v:roundrect style="width:' + circlesize + '; height:' + circlesize + '; behavior: url(#default#VML); display:inline-block; "  arcsize="'+circlesize+'" fillcolor="'+arrowcolor+'" strokecolor="'+arrowbordercolor+'" strokeweight="'+arrowbordersize+'"></v:roundrect>');
									self.arrow2.html('<v:roundrect style="width:' + circlesize2 + '; height:' + circlesize2 + '; behavior: url(#default#VML); display:inline-block;  "  arcsize="'+circlesize2+'" fillcolor="'+arrowcolor+'" strokecolor="'+arrowbordercolor+'" strokeweight="'+arrowbordersize+'"></v:roundrect>');
								}
								else
								{
									self.arrow.css({
										 'width': circlesize
										,'height': circlesize
										,'line-height': 0
										,'border': arrowbordersize+ 'px solid ' + arrowbordercolor
										,'background': arrowcolor
										,'border-radius': circlesize+'px'
										,'-moz-border-radius': circlesize+'px'
										,'-webkit-border-radius': circlesize+'px'
										,'-ms-border-radius': circlesize+'px'
									});

									self.arrow2.css({
										 'width': circlesize2
										,'height': circlesize2
										,'line-height': 0
										,'border': arrowbordersize+ 'px solid ' + arrowbordercolor
										,'background': arrowcolor
										,'border-radius': circlesize2+'px'
										,'-moz-border-radius': circlesize2+'px'
										,'-webkit-border-radius': circlesize2+'px'
										,'-ms-border-radius': circlesize2+'px'
									});
								}
							}
						};

						var _setBorderArrowStyle = function() {
							//ボーダーに食い込んだやじるしの片側系
							//二等辺三角形ではなく半分の長辺三角形を描画する場合、
							//ボーダーの片側がなくなってしまうので、ボーダーを2倍にして計算する.
							var halftype = _getArrowHalfType();
							var arrowsize = _getArrowSize();
							var arrowbordersize = _getArrowBorderSize();
							var arrowstyle = _getArrowStyle();

							var top = 0,left = 0;		//四隅の幅,
							var top2 = 0,left2 = 0;		//四隅の幅,
							var colorborder = '';		//色をつけるボーダー
							var deleteborder = '';		//直角三角形にするために削除するボーダー

							if (arrowstyle[0] == 'top' || arrowstyle[0] == 'bottom') {
								if (arrowstyle[0] == 'top') {
									top = -(arrowsize*2);
									top2 = top+(arrowbordersize*2);
									colorborder = 'bottom';

									if (halftype){
										top -= arrowbordersize*2;
									}
								} else {
									top = self.tooltip.height() ;
									top2 = top;
									colorborder = 'top';

									if (halftype){
										top += arrowbordersize;
									}
								}
								if (halftype){
									deleteborder = halftype == 1 ? 'left' : 'right';
								}

								if (arrowstyle[1] == 'left') {
									left = 0;
								} else if (arrowstyle[1] == 'middle') {
									left = Math.floor((self.tooltip.width() - arrowsize) / 2);
								} else { //right
									left = self.tooltip.width() - arrowsize*2 ;
								}
								left2 = left + arrowbordersize;
							} else { //left or right
								if (arrowstyle[0] == 'left') {
									left = -arrowsize*2;
									left2 = left + arrowbordersize*2;
									colorborder = 'right';

									if (halftype){
										left -= arrowbordersize*2;
									}
								} else {
									left = self.tooltip.width() ;
									left2 = left ;
									colorborder = 'left';

									if (halftype){
										left += arrowbordersize;
									}
								}
								if (halftype){
									deleteborder = halftype == 1 ? 'bottom' : 'top';
								}

								if (arrowstyle[1] == 'top') {
									top = 0;
								} else if (arrowstyle[1] == 'middle') {
									top = Math.floor((self.tooltip.height() - arrowsize) / 2);
								} else { //bottom
									top = self.tooltip.height() - arrowsize*2 ;
								}
								top2 = top + arrowbordersize;
							}

							var transparentColor = _getTransparentColor();
							self.arrow.css({
								 'width': 0
								,'height': 0
								,'line-height': 0
								,'border': arrowsize+ (halftype ? arrowbordersize : 0 ) + 'px solid ' + transparentColor
								,'position': 'absolute'
								,'top': top + self.options.arrowoffset.y + 'px'
								,'left': left + self.options.arrowoffset.x + 'px'
							}).css('border-'+colorborder+'-color',_getArrowColor() );

							self.arrow2.css({
								 'width': 0
								,'height': 0
								,'line-height': 0
								,'border': arrowsize - arrowbordersize + 'px solid ' + transparentColor
								,'position': 'absolute'
								,'top': top2 + self.options.arrowoffset.y + 'px'
								,'left': left2 + self.options.arrowoffset.x + 'px'
								,'zIndex': self.arrow.css('zIndex') + 1 //手前に
							}).css('border-'+colorborder+'-color', self.tooltip.css('background-color') );
							if (transparentColor != 'transparent'){
								//IE6 bug fix!
								self.arrow.get(0).style.filter = "chroma(color=" + transparentColor + ");"
								self.arrow2.get(0).style.filter = "chroma(color=" + transparentColor + ");"
							}
							if (halftype){
								//直角三角形にするために辺を削除する.
								self.arrow.css('border-' + deleteborder,'none');
								self.arrow2.css('border-' + deleteborder,'none');
							}
						};

						//プレーンな矢印の描画
						var _setArrowStyle = function(){
							var halftype = _getArrowHalfType();
							var arrowsize = _getArrowSize();
							var arrowbordersize = _getArrowBorderSize();
							var arrowstyle = _getArrowStyle();

							var top = 0,left = 0;		//四隅の幅,
							var colorborder = '';		//色をつけるボーダー
							var deleteborder = '';		//直角三角形にするために削除するボーダー
							if (arrowstyle[0] == 'top' || arrowstyle[0] == 'bottom') {
								if (arrowstyle[0] == 'top') {
									top = -(arrowsize*2+arrowbordersize);
									colorborder = 'bottom';
								} else {
									top = self.tooltip.height() + arrowbordersize;
									colorborder = 'top';
								}
								if (halftype){
									deleteborder = halftype == 1 ? 'left' : 'right';
								}

								if (arrowstyle[1] == 'left') {
									left = -arrowbordersize;
								} else if (arrowstyle[1] == 'middle') {
									left = Math.floor((self.tooltip.width() - arrowsize) / 2)-arrowbordersize;
								} else { //right
									left = self.tooltip.width() - arrowsize*2 - arrowbordersize;
								}
							} else { //left or right
								if (arrowstyle[0] == 'left') {
									left = -arrowsize*2-arrowbordersize;
									colorborder = 'right';
								} else {
									left = self.tooltip.width() + arrowbordersize;
									colorborder = 'left';
								}
								if (halftype){
									deleteborder = halftype == 1 ? 'bottom' : 'top';
								}

								if (arrowstyle[1] == 'top') {
									top = -arrowbordersize;
								} else if (arrowstyle[1] == 'middle') {
									top = Math.floor((self.tooltip.height() - arrowsize) / 2)-arrowbordersize;
								} else { //bottom
									top = self.tooltip.height() - arrowsize*2 - arrowbordersize;
								}
							}
							
							var transparentColor = _getTransparentColor();
							self.arrow.css({
								 'width': 0
								,'height': 0
								,'line-height': 0
								,'border': arrowsize + 'px solid ' + transparentColor
								,'position': 'absolute'
								,'top': top + self.options.arrowoffset.y + 'px'
								,'left': left + self.options.arrowoffset.x + 'px'
							}).css('border-'+colorborder+'-color', _getArrowColor() );
							if (transparentColor != 'transparent'){
								//IE6 bug!
								self.arrow.get(0).style.filter = "chroma(color=" + transparentColor + ");"
							}
							if (halftype){
								//直角三角形にするために辺を消す.
								self.arrow.css('border-' + deleteborder,'none');
							}
						};

						//矢印の色
						var _getArrowColor = function() {
							//ボーダーがあるならボーダーの色
							//ボーダーがないなら背景色
							return _getArrowBorderSize() > 0 ? 
								self.tooltip.css('border-top-color') : self.tooltip.css('background-color');
						};

						//矢印のサイズ
						var _getArrowSize = function() {
							var arrowstyle = _getArrowStyle();
							var arrowbordersize = _getArrowBorderSize();

							return self.options.arrowsize || 
								Math.max(parseInt((arrowstyle[0] == 'top' || arrowstyle[0] == 'bottom') ? 
										(self.tooltip.width()+arrowbordersize) / 20 
									:	(self.tooltip.height()+arrowbordersize) / 20)+arrowbordersize,1+arrowbordersize);
						};

						//矢印のボーダーのサイズ
						var _getArrowBorderSize = function() {
							return parseInt(self.tooltip.css('border-top-width')) || 0;
						};

						//通過色
						var _getTransparentColor = function() {
							return ($.browser.msie && $.browser.version < 7) ? 'fuchsia' : 'transparent';
						};

						//矢印の種類
						var _getArrowStyle = function() {
							var arrowstyle = self.options.arrowstyle.split( " " );
							if (!arrowstyle[1]) arrowstyle[1] = '';

							return arrowstyle;
						};

						//矢印が半分だけモードかどうかを取得する
						//halftype == 0  二等辺三角形、正三角形の矢印
						//halftype == 1  直角三角形1
						//halftype == 2  直角三角形2
						var _getArrowHalfType = function() {
							return self.options.arrowtype.indexOf('half2') != -1 ? 2 : (self.options.arrowtype.indexOf('half') != -1 ? 1 : 0);
						};

						//位置合わせ
						var _setPosition = function(){
							var arrowstyle = _getArrowStyle();
							var pos = $.extend({
								of: self.element
							}, self.options.position );

							if (pos.my && pos.my == 'auto' && pos.at && pos.at == 'auto' ){

								if (arrowstyle[0] == 'top'){
									if (arrowstyle[1] == 'left'){
										pos.my = 'left top';
										pos.at = 'left bottom';
									} else if (arrowstyle[1] == 'middle') {
										pos.my = 'middle top';
										pos.at = 'middle bottom';
									} else { //right
										pos.my = 'right top';
										pos.at = 'right bottom';
									}
								} else if (arrowstyle[0] == 'bottom'){
									if (arrowstyle[1] == 'left'){
										pos.my = 'left bottom';
										pos.at = 'left top';
									} else if (arrowstyle[1] == 'middle') {
										pos.my = 'middle bottom';
										pos.at = 'middle top';
									} else { //right
										pos.my = 'right bottom';
										pos.at = 'right top';
									}
								} else if (arrowstyle[0] == 'left'){
									if (arrowstyle[1] == 'top'){
										pos.my = 'left top';
										pos.at = 'right top';
									} else if (arrowstyle[1] == 'middle') {
										pos.my = 'left middle';
										pos.at = 'right middle';
									} else { //bottom
										pos.my = 'left bottom';
										pos.at = 'right bottom';
									}
								} else { //right
									if (arrowstyle[1] == 'top'){
										pos.my = 'right top';
										pos.at = 'left top';
									} else if (arrowstyle[1] == 'middle') {
										pos.my = 'right middle';
										pos.at = 'left middle';
									} else { //bottom
										pos.my = 'right bottom';
										pos.at = 'left bottom';
									}
								}
							}

//							self.tooltip
//								.show()				//positionで位置を計算するのに必要.
//								.position( pos )
//								.hide();
							self.pposition(pos);
						};


						//スタイルによって変形
						switch(this.options.arrowtype){
							case 'none':
								break;
							case 'mark':
							case 'bubble':
								_setMarkStyle();
								break;
							default:
								if (this.options.arrowtype.indexOf('border-arrow') != -1){
									_setBorderArrowStyle();
								} else {
									_setArrowStyle();
								}
								break;
						}
						//位置合わせ
						_setPosition();
					},


					_create: function() {
						this.tooltip = $("<div></div>");
						if ( this.options.content.jquery )
						{
							//ユーザが作ったjqueryオブジェクトがツールチップになる.
							this.tooltip.html(this.options.content);
						}
						else
						{
							//文字列として生成する.
							this.tooltip.html( $.isFunction( this.options.content ) ? 
									this.options.content.apply(this.element) : this.options.content
									);
						}

						//ツールチップの土台の修飾.
						this.tooltip
							.attr("id", "ui-tooltip-" + increments++)
							.attr("role", "tooltip")
							.attr("aria-hidden", "true")
							.addClass("ui-tooltip ui-widget ui-corner-all ui-widget-content " + this.options.className)
							.appendTo(document.body)
							.css({
								 position: 'absolute'
								,display: 'none'
								,'z-index': 50 
							})
							.css(this.options.css);

						//吹出し.
						this.arrow = $("<div></div>")
							.addClass("ui-tooltip-arrow")
							.appendTo(this.tooltip);
						this.arrow2 = this.arrow.clone()
							.appendTo(this.tooltip);

						//吹き出しを表示するイベントのセット.
						$.isFunction( this.options.showoption ) ? 
							this.options.showoption.apply(this) : 
							this._bindevent(this.options.showoption);
					},
					
					_bindevent: function(showoption)
					{
						if (showoption == "none")
						{
							return ;
						}

						var self = this;
						this.tooltip
							.bind("mouseover focus",function(event){ self.show(event); return false; })
							.bind("mouseout blur" ,function(event){ self.hide(event); return false; })
						if (showoption == "click")
						{
							this.element
								.bind("click"    ,function(event){ self.show(event); return false; })
								.bind("mouseout" ,function(event){ self.hide(event); return false; })
						}
						else
						{
							this.element
								.bind("mouseover focus",function(event){ self.show(event); return false; })
								.bind("mouseout blur" ,function(event){ self.hide(event); return false; })
						}
					},
					
					//表示する.
					show: function(event) {
						//無効なら表示しない.
						if (this.options.disabled){
							return ;
						}
						//遅延非表示タイマーが有効なら、もう今すぐ表示するから無効にする.
						if (this.hideDelayTimer){
							 clearTimeout(this.hideDelayTimer);
						}
						//現在そもそも表示しているか？
						if (this.shown) {
							//すでに表示済み.
							return;
						}
						//表示フラグを有効にする.
						this.shown = true;

						//吹き出しの描画条件の調整
						this._arrowStyleSetting();

						//表示用の関数の実行
						$.isFunction( this.options.show ) ? 
							this.options.show.apply(this.tooltip) : 
							this.tooltip.show(this.options.show);

						//表示されたと通知する.
						this._trigger( "open", event );
					},
					
					//非表示にする
					hide: function(event) {
						var self = this;

						//遅延表示タイマーが有効作り直す.
						if (this.hideDelayTimer){
							 clearTimeout(this.hideDelayTimer);
						}
						//タイマーを作ってゆっくり消す。
						//すぐに消してしまうと、ツールチップにフォーカスをできないため。
						this.hideDelayTimer = setTimeout(function () {
							this.hideDelayTimer = null;
							self.shown = false;

							$.isFunction( self.options.hide ) ? 
								self.options.hide.apply(self.tooltip) : 
								self.tooltip.hide(self.options.hide);
							self._trigger( "close", event );
						}, this.options.hideDelayTime );
					},

					//消す.
					_destroy: function() {
						this.arrow.remove();
						this.arrow2.remove();
						this.tooltip.remove();
					},

					//有効化
					enable: function() {
						this.options.disabled = false;
					},
					//無効化
					disable: function() {
						this.options.disabled = true;
						this.trigger('hide');
					},

					//今すぐ閉じる.
					close: function(event) {
						if (this.hideDelayTimer) clearTimeout(this.hideDelayTimer);
						this.tooltip.stop(false, true).hide();
						this._trigger( "close", event );
					}
					//イベントの発動
					,_trigger: function(method , event) {
						return sself.trigger(method , event);
					}
					//簡易場所合わせ
					,pposition: function(pos) {

						//対象物の絶対座標
						var abxy = pos.of.offset();

						//ターゲットこのオブジェクトに自分を合わせたい
						var atA = pos.at.split( " " );
						if (atA[0] == 'left'){
							//nop
						} else if (atA[0] == 'right'){
							abxy.left += pos.of.width();
						} else { //middle
							abxy.left += pos.of.width() / 2;
						}
						if (atA[1] == 'top'){
							//nop
						} else if (atA[1] == 'bottom'){
							abxy.top += pos.of.height();
						} else { //middle
							abxy.top += pos.of.height() / 2;
						}

						//自分のどのへんを合わせたいのか
						this.tooltip.show();
						var myA = pos.my.split( " " );
						if (myA[0] == 'left'){
							//nop
						} else if (myA[0] == 'right') {
							abxy.left -= this.tooltip.width();
						} else { //middle
							abxy.left -= this.tooltip.width() / 2;
						}
						if (myA[1] == 'top'){
							//nop
						} else if (myA[1] == 'bottom') {
							abxy.top -= this.tooltip.height();
						} else { //middle
							abxy.top -= this.tooltip.height() / 2;
						}

						//座標調整
						if (typeof pos.offset != 'undefined') {
							var atOffset = pos.offset.split( " " );
							abxy.left += parseInt(atOffset[0]);
							abxy.top += parseInt(atOffset[1]);
						}

						//設定.
						this.tooltip.css({
							 'position': 'absolute'
							,'left':     abxy.left
							,'top' :     abxy.top
						});
						this.tooltip.hide();
					},
					
					//設定
					config: function(key1,key2){
						if (typeof key2 == 'undefined'){
							if (typeof key1 == "object" && key1 != null) {
								//設定の一括指定
								this.options = $.extend(this.options,key1 );
							} else {
								//取得
								return this.options[ key1 ];
							}
						} else {
							//設定
							this.options[ key1 ] = key2;
						}
						return this;
					}
				};

			//オプションの初期化
			obj.options = $.extend(obj.options,method );
			obj.element = this;
			//初期ルーチンの呼び出し
			obj._create();
			//インスタンスの記録.
			sself.data(name , obj );
			return this;
		}

		var obj = sself.data(name);
		if (typeof method == 'undefined')
		{
			return this;
		}
		else if (typeof option == 'undefined')
		{
			if (method == "content") {
				return obj.tooltip;
			}
			return obj[method];
		}
		else if (typeof option2 == 'undefined')
		{
			if (method == "content") {
				obj.tooltip = option;
				return obj.tooltip;
			}
			return obj.apply(method, [ option ]);
		}
		return obj.apply(obj, [ option , option2 ]);
	};

})(jQuery);