
-- 実行されるごとに呼ばせれます
-- 引数は8個まで使えます。引数はすべて lua文字列で渡されます。
-- 戻り値は特にありません。
function call()
	onvoice("ディスプレイ(?:つけて|オン)"	, function() action("action__display_on") end);
	onvoice("ディスプレイ(?:けして|オフ)"	, function() action("action__display_off") end);
	onvoice("えきしょう(?:つけて|オン)"	, function() action("action__display_on") end);
	onvoice("えきしょう(?:けして|オフ)"	, function() action("action__display_off") end);
end
