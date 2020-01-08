#pragma once

struct XUdpSocket
{
	int protocol;
	bool isVoiceSocket;
	short port; // port stored in network byte order
};