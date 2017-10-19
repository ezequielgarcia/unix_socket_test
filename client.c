/*
 * Based on Michael Kerrisk's code.
 * See http://man7.org/tlpi/code/online/dist/sockets/ud_ucase_cl.c.html.
 *
 * GPL.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/mailbox"
#define BUF_SIZE 10

int
main(int argc, char *argv[])
{
    struct sockaddr_un sa, pa;
    int sfd, j;
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        printf("%s msg...\n", argv[0]);

    sfd = socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (sfd == -1) {
        perror("socket");
	exit(1);
    }

    memset(&pa, 0, sizeof(struct sockaddr_un));
    pa.sun_family = AF_UNIX;
    snprintf(pa.sun_path, sizeof(pa.sun_path),
	     "%s.%ld", SOCK_PATH, (long) getpid());

    if (bind(sfd, (struct sockaddr *) &pa, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
	exit(1);
    }

    memset(&sa, 0, sizeof(struct sockaddr_un));
    sa.sun_family = AF_UNIX;
    strncpy(sa.sun_path, SOCK_PATH, sizeof(sa.sun_path) - 1);

    for (j = 1; j < argc; j++) {
        msgLen = strlen(argv[j]);

        printf("[%d] sending <%.*s>\n", j, (int) msgLen, argv[j]);
        if (sendto(sfd, argv[j], msgLen, 0, (struct sockaddr *) &sa,
                sizeof(struct sockaddr_un)) != msgLen) {
            printf("sendto failed\n");
	    continue;
	}

	numBytes = 0;
	for (int tries = 100; tries; tries--) {
		numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
		if (numBytes > 0)
			break;
		usleep(50*1000);
	}
	if (numBytes)
	        printf("[%d] response <%.*s>\n",
		       j, (int)numBytes, resp);
    }
    remove(pa.sun_path);
    return 0;
}
