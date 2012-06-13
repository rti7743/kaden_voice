
-- 実行されるごとに呼ばせれます
-- 引数は8個まで使えます。引数はすべて lua文字列で渡されます。
-- 戻り値は特にありません。
function call()
	onvoice("エアコン(?:つけて|オン)"	, function(){
		speak("エアコンつけるよっ" , function(){
			action("action__aircon_cooler_on");
		})
	});
	onvoice("エアコン(?:つけて|オン)"	, function() action("action__aircon_cooler_on") end);
	onvoice("エアコン(?:けして|オフ)"	, function() action("action__aircon_cooler_off") end);
	onvoice("エアコンマックス"			, function() action("action__aircon_cooler_max") end);
	onvoice("クーラー(?:つけて|オン)"	, function() action("action__aircon_cooler_on") end);
	onvoice("クーラー(?:けして|オフ)"	, function() action("action__aircon_cooler_off") end);
	onvoice("クーラーマックス"			, function() action("action__aircon_cooler_max") end);
	onvoice("だんぼう(?:つけて|オン)"	, function() action("action__aircon_heater_on") end);
	onvoice("だんぼう(?:けして|オフ)"	, function() action("action__aircon_heater_off") end);
	onvoice("だんぼうマックス"			, function() action("action__aircon_heater_max") end);
	
end
