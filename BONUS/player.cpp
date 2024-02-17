#include "../INC/Player.hpp"

Player::Player(){
    movesLeft = 9;
    isPlaying = false;
    nickname = "";
}
Player::~Player(){}
Player::Player(const Player& other){*this = other;}

Player& Player::operator=(const Player& other)
{
	if (this != &other){
    	movesLeft = other.movesLeft;
    	isPlaying = other.isPlaying;
    	nickname = other.nickname;
    	board = other.board;
    	cmd = other.cmd;
    	board = other.board;
	}
    return *this;
}

void Player::setIsPlaying(bool value){isPlaying = value;}
void Player::setNickname(std::string nick){nickname = nick;}
void Player::setChatAtPos(int pos, char character) {board[pos] = character;}

int Player::getMoves() {return movesLeft;}
bool Player::getIsPlaying(){return isPlaying;}
char Player::getChatAtPos(int move){return board[move];}
std::string Player::getNickname(){return nickname;}

void Player::dicMoves() {movesLeft--;}
void Player::restMoves() {movesLeft = 9;}