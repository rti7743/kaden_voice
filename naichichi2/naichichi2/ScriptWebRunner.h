#pragma once
#include "Callbackable.h"
#include "HttpServer.h"
#include "XLHttpHeader.h"

class ScriptWebRunner : public Callbackable
{
public:
	ScriptWebRunner();
	virtual ~ScriptWebRunner();
	void Create(MainWindow * poolMainWindow);

	virtual std::string callbackFunction(const CallbackDataStruct* callback,const std::map<std::string , std::string> & match);
	bool Load();
	//インスタンスの再読み込み
	//プログラムをデバッグしているときとか、再読み込み機能がないと死ねるから・・・
	bool Reload();

	//webからアクセスがあった時.
	bool WebAccess(const std::string& path,const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,std::string* responsString) ;

private:
	MainWindow * PoolMainWindow;


	//リモコンのweb index
	std::string RemoconIndex(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	//実行!!
	std::string remocon_fire_byid(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	//実行!!
	std::string remocon_fire_bytype(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	//アイコンの並び順の更新
	std::string remocon_update_icon_order(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	//アクションアイコンの並び順の更新
	std::string remocon_update_elec_action_order(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	//エラーを返す
	std::string RemoconStatusError(int code ,std::string msg)  const;
	//ステータスを jsonで
	std::string RemoconStatus()  const;
	//機材のアップデート
	//editable_key=123&name=名前&type=タイプ&type_other=タイプその他
	std::string remocon_update_elec(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//機材のアクション項目のアップデート
	//actioneditable_key1=123&actioneditable_key2=456&actionname=名前&actiontype=種類&actiontype_other=種類その他&actionvoice=1&actionvoice_command=こんぴゅーた&showremocon=1&useapi=1&useinternet=1&topeak=1&topeak_command=電気つけます&actionexecuteflag=赤外線&actionexecuteflag_ir=12345678910&actionexecuteflag_command=a.exe&actionexecuteflag_command_args1=&actionexecuteflag_command_args2=&actionexecuteflag_command_args3=&actionexecuteflag_command_args4=&actionexecuteflag_command_args5=
	std::string remocon_update_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//家電操作処理の実行
	void fireAction(int key1,int key2)  const;
	//家電を消す
	std::string remocon_delete_elec(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//家電の操作を消す
	std::string remocon_delete_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) ;
	//家電の新しいIDを降り出す
	int NewElecID() const;
	//家電操作の新しいIDを降り出す
	int NewElecActionID(int eleckey) const;
	//機材の種類が重複していたらエラー
	bool checkUniqElecType(int key1,const std::string& type) const;
	//機材操作の種類が重複していたらエラー
	bool checkUniqElecActionType(int key1,int key2,const std::string& actiontype) const;
	//新しい機材の並び順を求める(末尾に追加する)
	int newOrderElec() const;
	//新しい機材操作の並び順を求める(末尾に追加する)
	int newOrderElecAction(int key1) const;
	//リファラーをチェックします。
	bool checkReferer(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,std::string* responsString) const;

	//設定のweb index
	std::string SettingIndex(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	//設定のステータスを jsonで
	std::string SettingStatus()  const;
	std::string setting_update_setting_account(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	std::string setting_update_setting_network(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	std::string setting_update_setting_recong(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	std::string setting_update_setting_speak(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	std::string remocon_fileselectpage_find(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	std::string remocon_fileselectpage_upload(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	std::string remocon_fileselectpage_delete(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;
	//音声認識エンジンをアップデートする
	std::string remocon_recongupdate(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const;

	mutable boost::mutex updatelock;
};
