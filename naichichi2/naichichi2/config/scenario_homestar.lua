
-- 実行されるごとに呼ばせれます
-- 引数は8個まで使えます。引数はすべて lua文字列で渡されます。
-- 戻り値は特にありません。
function call()
	onvoice("ホームスター(?:つけて|オン)"	, function() action("action__homestar_on") end);
	onvoice("ホームスター(?:けして|オフ)"	, function() action("action__homestar_off") end);

	webmenu("ホームスター","electronics.jpg");
	webmenusub("ホームスター","OFF","electronics.jpg",function() action("action__homestar_off") end);
	webmenusub("ホームスター","ON","electronics.jpg",function() action("action__homestar_on") end);
end
