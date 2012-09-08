#include "common.h"
#include "XLVolumeControl.h"

//for vista
#include <atlbase.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Audioclient.h>


int XLVolumeControl::GetAudioVolume()
{
	
}
void XLVolumeControl::SetAudioVolume(int volumne)
{
	//http://www.selena-net.com/~piabrpg/mata-ri/log_0701.html
	HRESULT hr;

	//We initialize the device enumerator here
	CComPtr<IMMDeviceEnumerator>		deviceEnumerator;
	hr = deviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	//Here we enumerate the audio endpoints of interest (in this case audio capture endpoints)
	//into our device collection. We use "eCapture"
	//for audio capture endpoints, "eRender" for 
	//audio output endpoints and "eAll" for all audio endpoints 
	CComPtr<ISimpleAudioVolume>		deviceCollection;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,&deviceCollection);
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	//メインボリュームの音量を変更 ｆLevelの範囲は0.0～1.0
	hr = deviceCollection->SetMasterVolumeLevelScalar( fLevel, &GUID_NULL );
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	return 0;
}

int XLVolumeControl::GetMicrophoneVolume()
{
	//http://social.msdn.microsoft.com/Forums/en-US/windowspro-audiodevelopment/thread/81de9224-d155-41dc-ae3f-cfb4f60fc5b8
	HRESULT hr;

	//We initialize the device enumerator here
	CComPtr<IMMDeviceEnumerator>		deviceEnumerator;
	hr = deviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	//Here we enumerate the audio endpoints of interest (in this case audio capture endpoints)
	//into our device collection. We use "eCapture"
	//for audio capture endpoints, "eRender" for 
	//audio output endpoints and "eAll" for all audio endpoints 
	CComPtr<IMMDeviceCollection>		deviceCollection;
	hr = deviceEnumerator->EnumAudioEndpoints( eCapture, DEVICE_STATE_ACTIVE, &deviceCollection );
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	UINT deviceCount;
	hr = deviceCollection->GetCount(&deviceCount);
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	//
	//This loop goes over each audio endpoint in our device collection,
	//gets and diplays its friendly name and then tries to mute it
	//
	for (UINT i = 0 ; i < deviceCount ; i += 1)
	{
		LPWSTR deviceName;

		//Here we use the GetDeviceName() function provided with the sample 
		//(see source code zip)
		deviceName = GetDeviceName(deviceCollection, i); //Get device friendly name

		if (deviceName == NULL) break;

		printf("Device to be muted has index: %d and name: %S\n", i, deviceName);

		//this needs to be done because name is stored in a heap allocated buffer
		free(deviceName);

		device = NULL;

		//Put device ref into device var
		CComPtr<IMMDevice>		device;
		hr = deviceCollection->Item(i, &device);
		if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

		//This is the Core Audio interface of interest
		CComPtr<IAudioEndpointVolume>		endpointVolume;
		IAudioEndpointVolume *endpointVolume = NULL;

		//We activate it here
		hr = device->Activate( __uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, &endpointVolume );
		if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

//		hr = endpointVolume->SetMute(TRUE, NULL); //Try to mute endpoint here
//		if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));
		
		break;
	}

	return 0;
}

void XLVolumeControl::SetMicrophoneVolume(int volumne)
{
	//http://www.codeproject.com/Articles/437537/Muting-all-microphones-on-Vista-Win7
	
}
