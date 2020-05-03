#pragma once


#if COMPILE_WITH_VOICE
#include "3rdparty/portaudio/include/portaudio.h"

enum DeviceType : BYTE
{
	Input,
	Output
};

struct DeviceInfo
{
	DeviceType			deviceType;
	const PaDeviceInfo	*deviceInfo = nullptr;
	const PaHostApiInfo *hostApiInfo = nullptr;
	PaError				deviceError = paNoError;
};

class CAudioDevices
{
public:

	CAudioDevices();

	void SelectAudioOutput();
	void SelectAudioInput();

	/*	Default input/output device indexes	*/
	int					m_deviceCount;
	PaDeviceIndex		m_defaultOutputDeviceIndex, m_defaultInputDeviceIndex = -1;

	PaError				GetAudioInterfaceError() { return m_lastDeviceErr; }
	bool				IsDeviceAvailable(DeviceType devType) { return devType == Input ? (m_defaultInputDeviceIndex != -1 ? true : false) : (m_defaultOutputDeviceIndex != -1 ? true : false); }
	DeviceInfo*			GetDeviceInfo(PaDeviceIndex index) { if (index < m_deviceCount) return m_audioDeviceInfoArray; else return nullptr; }

	~CAudioDevices();

private:

	PaError				m_lastDeviceErr = paNoError;
	DeviceInfo*			m_audioDeviceInfoArray = nullptr;
	bool				m_defaultInputFound, m_defaultOutputFound = false;

};


#endif // !1



