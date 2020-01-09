#pragma once

#define IPPROTO_VDP 254

struct XSocket
{
	short port; // port stored in network byte order
	int protocol;
	bool isVoiceSocket;
	SOCKET WinSockHandle;
};