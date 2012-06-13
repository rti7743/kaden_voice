/*
julius をMS-SAPIレベルぐらいに楽に使うために、我々はもっとハックをしなければならない。


julius「マナの本当の力を思い知れ!!」
*/
#include "JuliusPlus.h"
#include "JuliusPlusRule.h"
#include "ActionImplement.h"

int main()
{
	//i-remoconのIPアドレス
	const std::string iremoconIP = "192.168.10.2";
	//i-remoconのport番号
	const std::string iremoconPort = "51013";
	//i-remoconに送信するコマンド
	const std::string aircon_on  = "*is;18\r\n"; //エアコンを付けるコマンド
	const std::string aircon_off = "*is;14\r\n"; //エアコンを消すコマンド

	std::cout << "julius-plus 始まります。" << std::endl;
	{
		JuliusPlus julius;
		julius.Create(NULL);

		//採用する認識率と 二重検知を有効にするかを設定します。
		julius.SetRecognitionFilter(0.70,0.70,0.70,true);

		//呼びかけは「コンピュータ」で開始します。
		std::list<std::string> yobilist;
		yobilist.push_back("コンピューター");
		yobilist.push_back("コンピュータ");
		julius.SetYobikake(yobilist);

		//コマンド群です。
		//コマンド群は、カタカナかひらがなで書いてください。
		//正規表現の記号が一部使えます。
		//		正規表現  (aaa|bbb)
		//                 ? や * . などは今は使えません。
		//
		julius.AddCommandRegexp("でんたく(きどう|じっこう)",CallbackDataStruct([=](){
			//電卓を起動するコマンドをココに書く
			std::cout << "###RUN>電卓起動するよ!!" << std::endl;
#ifdef _WINDOWS_
			ShellExecute(NULL,NULL,"calc.exe",NULL,NULL,0);
#else
			system("xcalc >/dev/null 2>&1 &");
#endif
		}));
		julius.AddCommandRegexp("エアコン(つけて|オン)",CallbackDataStruct([=](){
			//エアコンをつける処理をここに書きます。
			std::cout << "###RUN>エアコンつけるよ!!" << std::endl;

			//example
			//ActionImplement::Telnet(iremoconIP,iremoconPort,"",aircon_on,"");

		}));
		julius.AddCommandRegexp("エアコン(けして|オフ)",CallbackDataStruct([=](){
			//エアコンを消す処理をここに書きます。
			std::cout << "###RUN>エアコン消すよ!!" << std::endl;

			//example
			//ActionImplement::Telnet(iremoconIP,iremoconPort,"",aircon_off,"");
		}));
		julius.AddCommandRegexp("でんき(つけて|オン)",CallbackDataStruct([=](){
			//電気をつける処理をここに書きます。
			std::cout << "###RUN>でんきつけるよ!!" << std::endl;
		}));
		julius.AddCommandRegexp("でんき(けして|オフ)",CallbackDataStruct([=](){
			//電気を消す処理をここに書きます。
			std::cout << "###RUN>でんき消すよ!!" << std::endl;
		}));
		julius.AddCommandRegexp("おんがくかけて",CallbackDataStruct([=](){
			//音楽をかける処理をここに書きます。
			std::cout << "###RUN>おんがくかけるよ" << std::endl;
		}));
		julius.AddCommandRegexp("どいつむら",CallbackDataStruct([=](){
			//日本で一番有名なテーマパークに関する処理をここに書きます。
			std::cout << "###RUN>日本で一番有名なテーマパーク" << std::endl;
		}));

		julius.CommitRule();

		//キー入力待ち
		std::string line;
		getline( std::cin, line );

		std::cout << "終了させます。" << std::endl;
	}

//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	return 0;
}
