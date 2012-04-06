

-- 実行されるごとに呼ばせれます
-- 引数は8個まで使えます。引数はすべて lua文字列で渡されます。
-- 戻り値は特にありません。
function call()
	onvoice("もうふ(?:つけて|オン)"	, function() action("action__moufu_on") end);
	onvoice("もうふ(?:けして|オフ)"	, function() action("action__moufu_off") end);

	webmenu("もうふ","electronics.jpg");
	webmenusub("もうふ","OFF","electronics.jpg",function() action("action__moufu_off") end);
	webmenusub("もうふ","ON","electronics.jpg",function() action("action__moufu_on") end);
end
