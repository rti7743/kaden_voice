
function call(){
	onvoice("でんき(?:つけて|オン)"	, function(){
		speak("でんきつけます。" ,function(){
			action("action__light_on");
		});
	});
	onvoice("ひかりあれ"			, function(){
		action("action__light_on");
	});
	onvoice("ラナルータ"			, function(){
		action("action__light_on");
	});
	onvoice("でんき(?:けして|オフ)"	, function(){
		action("action__light_off");
	});

}
