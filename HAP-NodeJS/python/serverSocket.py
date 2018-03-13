# coding:utf-8
# /usr/bin/python

# creator = wangkai
# creation time = 2018/3/8 18:38

import socket
import select
import sys


if __name__ == "__main__":
    rlist = [sys.stdin]
    RECV_BUFFER = 4096
    HOST = "192.168.100.5"
    PORT = 8484

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))
    server_socket.listen(10)

    rlist.append(server_socket)


    while 1:
        read_sockets, write_sockets, error_sockets = select.select(rlist, [], [])

        for r in read_sockets:
            if r == server_socket:
                sockfd, addr = server_socket.accept()
                rlist.append(sockfd)
            elif r == sys.stdin:
                msg = sys.stdin.readline()
                for sock in rlist:
                    if sock != server_socket and sock != sys.stdin:
                        sock.send(msg)

            else:
                try:
                    data = r.recv(RECV_BUFFER)
                    if data:
                        # print data+"\n"
                        sys.stdout.write(data)
                    else:

                        r.close()
                        if r in rlist:
                            rlist.remove(r)
                        continue
                except:
                    pass

    server_socket.close()
