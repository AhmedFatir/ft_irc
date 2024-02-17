#pragma once

#include <iostream>
#include <vector>

class Player
{
private:
	int movesLeft;
	bool isPlaying;
	std::string nickname;
	std::string cmd;
public:
	std::vector<char> board;
	Player();
	Player(const Player& other);
	Player& operator=(const Player& other);
	~Player();

	void setIsPlaying(bool value);
	void setNickname(std::string nick);
	void setChatAtPos(int pos, char character);

	int getMoves();
	bool getIsPlaying();
	char getChatAtPos(int move);
	std::string getNickname();

	void dicMoves();
	void restMoves();
};
