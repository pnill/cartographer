#pragma once

class GameManager {
public:
	bool isHost(); 
	bool start();
private:
	bool started = false;
};