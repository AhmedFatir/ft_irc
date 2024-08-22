# ft_irc

### Description

ft_irc is a project developed as part of the curriculum at 42 school. It is an implementation of an Internet Relay Chat (IRC) server and client using the C++ programming language. IRC is a protocol for real-time messaging and communication in a distributed network environment.
My full explanation of the project is in this Medium article. I hope you will like it. [My Medium blog](https://medium.com/@afatir.ahmedfatir/small-irc-server-ft-irc-42-network-7cee848de6f9)

### Features
- Multi-threaded architecture for handling concurrent client connections.
- Support for multiple simultaneous connections.
- Creation and management of IRC channels.
- User authentication and registration.
- Broadcasting messages to all users in a channel.
- Private messaging between users.
- Handling of various IRC commands such as JOIN, PRIVMSG, PART, etc.
- Support for user nicknames and channel names.
- Connect to the IRC server.
- Join channels and participate in group conversations.
- Send and receive messages.
- Change user nickname.
- Send private messages to other users.

### Installation
``` bash
git clone <repository_url>
cd ft_irc
make
```
### Usage
``` bash
./ircserv <port number> <password>
```
So at this point, the server is running. It just needs to be connected to. You can use any IRC client or simply use the nc (NetCut) tool like this:
```bash
nc localhost <port number>
```
After that, authenticate your client using the following commands:
```bash
pass <password>
nick <nickname>
user <username> #it should be 4 arguments
```
### BONUS
For the bonus part, there's a client bot that offers 3 services: game, age, and Nokta. To use the bot, you can run:
```bash
make bonus
./bot
```
The bot takes its arguments from the .env file. You may modify the values if necessary. 
you can communicate with the bot using another client through PRIVMSG command like:
```bash
privmsg bot :age 1996-10-09
```
and the bot will respond:
```bash
:bot!~bot@localhost PRIVMSG afatir :Your Age is: 27 years, 5 months, 14 days old
```
or you can play an (X | O) game with the bot by simply sending a play message:
```bash
privmsg bot :play           
:bot!~bot@localhost PRIVMSG afatir :Welcome to (X | O) Game!
:bot!~bot@localhost PRIVMSG afatir :YOU : X | Computer: O
:bot!~bot@localhost PRIVMSG afatir :-----------
:bot!~bot@localhost PRIVMSG afatir : - | - | - 
:bot!~bot@localhost PRIVMSG afatir :-----------
:bot!~bot@localhost PRIVMSG afatir : - | - | - 
:bot!~bot@localhost PRIVMSG afatir :-----------
:bot!~bot@localhost PRIVMSG afatir : - | - | - 
:bot!~bot@localhost PRIVMSG afatir :-----------
```
and play the game through the PRIVMSG command.
or to get Nokta (joke):
```bash
privmsg bot :nokta
:bot!~bot@localhost PRIVMSG afatir :How long does it take a woman to take out the trash? Nine months.
```

## Usage example
#### Run this cmd in the terminal
```bash
./ircserv 4444 hello
```
#### in another terminal run
```bash
nc localhost 4444
```
#### and then pass the following commands
```bash
pass hello
nick afatir
user afatir 0 * afatir
```
### and for the bot it connects automatically to the server
```bash
./bot localhost 4444 hello bot BONUS/qoutes
```

## For the full explanation of the IRC Server, you can check out my [Medium blog](https://medium.com/@afatir.ahmedfatir/small-irc-server-ft-irc-42-network-7cee848de6f9) for the complete details.
