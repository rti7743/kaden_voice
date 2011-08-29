// RSpeechRecognition.h: RSpeechRecognition クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSPEECHRECOGNITION_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_RSPEECHRECOGNITION_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"
#include "RComException.h"
#include <atlbase.h>
#include <sapi.h>
#include <sphelper.h>

class RSpeechRecognition  
{
public:
	RSpeechRecognition();
	virtual ~RSpeechRecognition();
	//音声認識のためのオブジェクトの構築.
	void RSpeechRecognition::Create(const string & inToWave = "") throw(RComException);
	void RSpeechRecognition::Create(const string & inToWave = "",const string & inGrammarXML = "") throw(RComException);

	//認識開始
	void RSpeechRecognition::Listen() throw(RComException);

	const string & getResultString() const
	{
		return ResultString;
	}
	const string  getResultMap(const string & inKey) const
	{
		RStringMap::const_iterator i = ResultMap.find(inKey);
		if (i == ResultMap.end() ) return "";

		return (*i).second;
	}
private:
	CComPtr<ISpRecognizer>		Engine;			// 認識エンジンオブジェクト
	CComPtr<ISpRecoContext>		RecoCtxt;

	
	CComPtr<ISpRecoGrammar>     DictationGrammar;	//メインとなる文法


	//認識結果
	string			ResultString;
	RStringMap		ResultMap;
};

#endif // !defined(AFX_RSPEECHRECOGNITION_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
