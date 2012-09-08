
-- 実行されるごとに呼ばせれます
-- 引数は8個まで使えます。引数はすべて lua文字列で渡されます。
-- 戻り値は特にありません。
function call(type,dir,filename)
	if (type == "title") then
	elseif (type == "artist") then
	elseif (type == "album") then
	elseif (type == "alias") then
	end
	return "";
end
