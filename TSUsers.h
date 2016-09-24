#ifndef TSUSERS_H
#define TSUSERS_H
#include <unordered_map>
#include "xliveless.h"

class TSUsers {
public:
	TSUsers();
	virtual ~TSUsers();

	void addUser(XUID id);
	void removeUser(XUID id);

	void userStartedTalking(XUID id);
	void userStoppedTalking(XUID id);

	void empty();

	BOOL isUserTalking(XUID id);

private:
	void setTalkingFlag(XUID id, BOOL talking);
	std::unordered_map<XUID, BOOL> xuidIsTalkingMap;
};
#endif