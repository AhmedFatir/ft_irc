#define once


#define DELIMITER "\r\n"

#define RPL_UMODEIS(hostname, channelname, mode, user)  ":" + hostname + " MODE " + channelname + " " + mode + " " + user + DELIMITER
#define RPL_CREATIONTIME(nickname, channelname, creationtime) ": 329 " + nickname + " #" + channelname + " " + creationtime + DELIMITER
#define RPL_CHANNELMODES(nickname, channelname, modes) ": 324 " + nickname + " #" + channelname + " " + modes + DELIMITER
#define RPL_CHANGEMODE(hostname, channelname, mode, arguments) (":" + hostname + " MODE #" + channelname + " " + mode + " " + arguments + DELIMITER)

///////// ERRORS ////////////////
#define ERR_NEEDMODEPARM(channelname, mode) ": 696 #" + channelname + " * You must specify a parameter for the key mode. " + mode + DELIMITER
#define ERR_INVALIDMODEPARM(channelname, mode) ": 696 #" + channelname + " Invalid mode parameter. " + mode + DELIMITER
#define ERR_KEYSET(channelname) ": 467 #" + channelname + " Channel key already set. " + DELIMITER
#define ERR_UNKNOWNMODE(nickname, channelname, mode) ": 472 " + nickname + " #" + channelname + " " + mode + " :is not a recognised channel mode" + DELIMITER
#define ERR_NOTENOUGHPARAM(nickname) (": 461 " + nickname + " :Not enough parameters." + DELIMITER)
#define ERR_CHANNELNOTFOUND(nickname, channelname) (": 403 " + nickname + " #" + channelname + " :No such channel" + DELIMITER)
#define ERR_NOTOPERATOR(channelname) (": 482 #" + channelname + " :You're not a channel operator" + DELIMITER)
#define ERR_NOSUCHNICK(channelname, name) (": 401 #" + channelname + " " + name + " :No such nick/channel" + DELIMITER )
