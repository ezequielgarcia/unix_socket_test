/*
 * Based on Michael Kerrisk's code.
 * See http://man7.org/tlpi/code/online/dist/sockets/ud_ucase_sv.c.html.
 *
 * GPL.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>

#define SOCK_PATH "/tmp/mailbox"
#define BUF_SIZE 10

int
main(int argc, char *argv[])
{
    struct sockaddr_un sa, pa;
    int sfd;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];
    struct timeval tv;

    sfd = socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (sfd == -1) {
        perror("socket");
	exit(1);
    }

    if (strlen(SOCK_PATH) > sizeof(sa.sun_path) - 1) {
        printf("Server socket path too long: %s", SOCK_PATH);
	exit(1);
    }

    if (remove(SOCK_PATH) == -1 && errno != ENOENT) {
        printf("remove-%s", SOCK_PATH);
	exit(1);
    }

    memset(&sa, 0, sizeof(struct sockaddr_un));
    sa.sun_family = AF_UNIX;
    strncpy(sa.sun_path, SOCK_PATH, sizeof(sa.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &sa, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
	exit(1);
    }

    for (;;) {
        len = sizeof(struct sockaddr_un);
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0,
			   (struct sockaddr *) &pa, &len);
        if (numBytes < 0) {
	    sleep(1);
            continue;
	}

        printf("server received %ld bytes from %s\n", numBytes, pa.sun_path);

        if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *) &pa, len) !=
	    numBytes)
            printf("sendto failed\n");
    }
}
