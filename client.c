#define _GNU_SOURCE
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>

#include "spliced_move.h"

#define SERV_PORT 20134

char *serv_addr = NULL;

static int get_sockaddr_in(struct sockaddr_in *addr) {
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	if (serv_addr == NULL) {
		err("No addr\n");
		return -1;
	}
	if (!inet_pton(AF_INET, serv_addr, &addr->sin_addr)) {
		err("Bad server address\n");
		return -1;
	}
	addr->sin_port = htons(SERV_PORT);
	return 0;
}

int main(int argc, char *argv[])
{
	int ret;
	int sk, ask = -1;
	struct sockaddr_in saddr;
	int in;

	if (argc < 2) {
		printf("Need to enter ip of server\n");
		return 0;
	}
	serv_addr = argv[1];

	printf("Starting client to connect %s on port %d\n", argv[1], SERV_PORT);

	sk = socket(PF_INET, SOCK_STREAM, 0);
	if (sk < 0) {
		perror("Failed to create socket");
		return -1;
	}

	ret = get_sockaddr_in(&saddr);
	if (ret < 0)
		goto out;

	ret = connect(sk, (struct sockaddr *)&saddr, sizeof(saddr));
	if (ret < 0) {
		perror("Can not connect to server");
		goto out;
	}

	printf("Connected to server\n");

	ret = in = open("tmpfs/in", O_RDONLY);
	if (ret < 0) {
		perror("open");
		goto out;
	}

	ret = spliced_move(in, sk);
	if (ret < 0)
		printf("Can not send file to socket\n");

	close(in);
out:
	printf("Exit\n");
	close(sk);
	return ret;
}
