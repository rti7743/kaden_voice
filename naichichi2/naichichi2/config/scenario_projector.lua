
-- 実行されるごとに呼ばせれます
-- 引数は8個まで使えます。引数はすべて lua文字列で渡されます。
-- 戻り値は特にありません。
function call()
	onvoice("プロジェクター(?:つけて|オン)"	, function() action("action__projector_on") end);
	onvoice("プロジェクター(?:けして|オフ)"	, function() action("action__projector_off") end);

	webmenu("プロジェクター","electronics.jpg");
	webmenusub("プロジェクター","OFF","electronics.jpg",function() action("action__projector_off") end);
	webmenusub("プロジェクター","ON","electronics.jpg",function() action("action__projector_on") end);
end
