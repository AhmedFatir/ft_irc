#pragma once

#include <iostream>

class Player
{
    private:
        std::string nickname;
        int movesLeft;
        std::string cmd;
        bool isPlaying;
    public:
        std::vector<char> board;
        Player();
        Player(const Player& other);
        Player& operator=(const Player& other);
        ~Player();

        void setNickname(std::string nick);
        std::string getNickname();
        bool getIsPlaying();
        void setIsPlaying(bool value);

        char getChatAtPos(int move)
        {
            return board[move];
        }
        void setChatAtPos(int pos, char character)
        {
            board[pos] = character;
        }
        int getMoves() {return movesLeft;}
        void dicMoves() {movesLeft--;}
        void restMoves() {movesLeft = 9;}


};

Player::Player()
{
    nickname = "";
    movesLeft = 9;
    isPlaying = false;
}

Player::Player(const Player& other)
{
    *this = other;
}
Player& Player::operator=(const Player& other)
{
    nickname = other.nickname;
    board = other.board;
    movesLeft = other.movesLeft;
    cmd = other.cmd;
    isPlaying = other.isPlaying;
    board = other.board;
    return *this;
}

Player::~Player()
{
}

void Player::setNickname(std::string nick)
{
    nickname = nick;
}

void Player::setIsPlaying(bool value)
{
    isPlaying = value;
}

std::string Player::getNickname()
{
    return nickname;
}

bool Player::getIsPlaying()
{
    return isPlaying;
}