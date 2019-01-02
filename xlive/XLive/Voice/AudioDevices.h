#pragma once

#include "Globals.h"

enum DeviceType : BYTE
{
	Input,
	Output
};

typedef struct DeviceInfo
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

	/*	Default input/output device indexes	*/		
	int					m_totalDevices;
	PaDeviceIndex		m_defaultOutputDeviceIndex, m_defaultInputDeviceIndex = -1;

	PaError				GetAudioClassError() { return m_CAudioErr; }
	bool				IsDeviceAvailable(DeviceType devType) { return devType == Input ? (m_defaultInputDeviceIndex != -1 ? true : false) : (m_defaultOutputDeviceIndex != -1 ? true : false); }
	DeviceInfo*			GetDeviceInfo(PaDeviceIndex index) { if (index < m_totalDevices) return m_audioDeviceInfoArray; else return nullptr; }

	~CAudioDevices();

private:

	PaError				m_CAudioErr = paNoError;
	DeviceInfo*			m_audioDeviceInfoArray = nullptr;
	bool				m_defaultInputFound, m_defaultOutputFound = false;

};

