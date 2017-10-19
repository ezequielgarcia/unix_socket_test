import socket
import time
import os

sock_path = "/tmp/mailbox"
pid = os.getpid()
buf_size = 10

s = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
s.setblocking(0)
s.bind("{}.{}".format(sock_path, pid))
while True:
    sent = s.sendto("123", sock_path)
    print("{} bytes sent".format(sent))

    rcvd = ""
    for x in range(0, 10):
        try:
            rcvd = s.recv(buf_size)
        except socket.error as error:
            time.sleep(0.5)
            pass
        else:
            break
    print("received <{}>".format(rcvd))
    time.sleep(2);

s.close()
