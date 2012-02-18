

-- 実行されるごとに呼ばせれます
-- 引数は8個まで使えます。引数はすべて lua文字列で渡されます。
-- 戻り値は特にありません。
function call()
	onvoice("でんき(?:つけて|オン)"	, function() action("action__light_on") end);
	onvoice("ひかりあれ"				, function() action("action__light_on") end);
	onvoice("ラナルータ"				, function() action("action__light_on") end);
	onvoice("でんき(?:けして|オフ)"	, function() action("action__light_off") end);
end
