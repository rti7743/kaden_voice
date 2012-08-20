
function call(){
	onvoice("でんき(?:つけて|オン)"	, function(){
		action("action__light_on");
--		speak("でんきつけます。" ,function(){
--			action("action__light_on");
--		});
	});
	onvoice("ひかりあれ"			, function(){
		action("action__light_on");
	});
	
	i = 0
	
	onvoice("ラナルータ"			, function(){
		if ((i % 2) == 1)
		{
			action("action__light_on");
		}
		else
		{
			action("action__light_off");
		}
		i = i + 1;
		
	});
	onvoice("でんき(?:けして|オフ)"	, function(){
		action("action__light_off");
	});

}
