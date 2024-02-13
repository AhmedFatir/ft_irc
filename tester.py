import socket
import threading
import time

# Informations de connexion au serveur IRC
# server_ip = "e1r4p13.1337.ma"
server_ip = "localhost"
server_port = 9009
server_password = "123"
# server_password = "zarda"

# Liste de pseudos disponibles
available_nicks = []


for i in range(1, 50):
    nick = "client{}".format(i)
    available_nicks.append(nick)

def connect_irc(nick):
    irc_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    irc_socket.connect((server_ip, server_port))

    # irc_socket.send("PASS {}\r\n".format(server_password).encode())

    irc_socket.send("NICK {}\r\n".format(nick).encode())
    irc_socket.send("USER {} 0 * :Client Bot\r\n".format(nick).encode())
    irc_socket.send("JOIN #ch\r\n".format(nick).encode())
    while True:
        irc_socket.send("PRIVMSG #ch :salam \r\n".format(nick).encode())

    while True:
        message = irc_socket.recv(2048).decode()
        print(message)

for nick in available_nicks:
    thread = threading.Thread(target=connect_irc, args=(nick,))
    thread.start()


# def connect_irc(nick):
#     irc_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     irc_socket.connect((server_ip, server_port))

#     irc_socket.send("PASS {}\r\n".format(server_password).encode())
#     irc_socket.send("NICK {}\r\n".format(nick).encode())
#     irc_socket.send("USER {} 0 * :Client Bot\r\n".format(nick).encode())
#     irc_socket.send("JOIN #ch\r\n".format(nick).encode())

#     while True:
#         irc_socket.send("PRIVMSG #ch :salam \r\n".format(nick).encode())
#         time.sleep(1)  # Add a delay to avoid excessive sending

#     while True:
#         message = irc_socket.recv(2048).decode()
#         print(message)

# def main():
#     threads = []

#     for nick in available_nicks:
#         thread = threading.Thread(target=connect_irc, args=(nick,))
#         threads.append(thread)
#         thread.start()

#     # Wait for all threads to finish
#     for thread in threads:
#         thread.join()

# if __name__ == "__main__":
#     main()