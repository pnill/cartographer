#include <windows.h>
#include <stdio.h>

#include "TSUsers.h"

TSUsers::TSUsers() {}

TSUsers::~TSUsers() {}

void TSUsers::addUser(XUID id) {
	xuidIsTalkingMap[id] = false;
}

void TSUsers::removeUser(XUID id) {
	xuidIsTalkingMap.erase(id);
}

void TSUsers::setTalkingFlag(XUID id, BOOL talking) {
	xuidIsTalkingMap[id] = talking;
}

void TSUsers::empty() {
	xuidIsTalkingMap.clear();
}

BOOL TSUsers::isUserTalking(XUID id) {
	if (!xuidIsTalkingMap.count(id)) {
		addUser(id);
	}
	return xuidIsTalkingMap[id];
}

void TSUsers::userStartedTalking(XUID id) {
	setTalkingFlag(id, true);
}

void TSUsers::userStoppedTalking(XUID id) {
	setTalkingFlag(id, false);
}