// 実行されるごとに呼ばせれます
function call()
{
	//家電制御の画面を表示
	onhttp("/electronics",function(request){
		local out = {}
		out["request"] = request
		
		out["webmenu"] = getwebmenu();
		out["nowroom"] = request["nowroom"];
		if (not out["nowroom"] or out["nowroom"] == "")
		{
		out["nowroom"] = "121212";
			//現在の部屋を選択
			for key,value in pairs(out["webmenu"]) do
				if ( value["ip"] == "") {
					out["nowroom"] = key;
					break;
				}
			end
		}
		
		webload("control_electronics.tpl",out);
		return ;
	});
	onhttp("/electronics_fire",function(request){
		local room = request["room"];
		local menu = request["menu"];
		local action = request["action"];
		callwebmenu(room,menu,action);
	});
}
