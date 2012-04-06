
-- 実行されるごとに呼ばせれます
-- 引数は8個まで使えます。引数はすべて lua文字列で渡されます。
-- 戻り値は特にありません。
function call()
	onvoice("こたつ(?:つけて|オン)"	, function() action("action__kotatsu_on") end);
	onvoice("こたつ(?:けして|オフ)"	, function() action("action__kotatsu_off") end);

	webmenu("こたつ","electronics.jpg");
	webmenusub("こたつ","OFF","electronics.jpg",function() action("action__kotatsu_off") end);
	webmenusub("こたつ","ON","electronics.jpg",function() action("action__kotatsu_on") end);
end
