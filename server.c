#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>

#include "spliced_move.h"

#define SERV_PORT 20134

static void get_sockaddr_in(struct sockaddr_in *addr) {
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(SERV_PORT);
	return;
}

int main(int argc, char *argv[])
{
	int ret;
	int sk, ask = -1;
	struct sockaddr_in saddr, caddr;
	socklen_t clen = sizeof(caddr);
	int out;

	printf("Starting server on port %d\n", SERV_PORT);

	sk = socket(AF_INET, SOCK_STREAM, 0);
	if (sk < 0) {
		perror("Failed to create socket");
		return -1;
	}

	get_sockaddr_in(&saddr);

	ret = bind(sk, (struct sockaddr *)&saddr, sizeof(saddr));
	if (ret != 0) {
		perror("Can not bind socket");
		goto out;
	}

	ret = listen(sk, 1);
	if (ret != 0) {
		perror("Can not listen on socket");
		goto out;
	}

	ret = ask = accept(sk, (struct sockaddr *)&caddr, &clen);
	if (ask < 0) {
		perror("Failed to accept connection");
		goto out;
	}

	printf("Accepted connection\n");

	ret = out = open("tmpfs/out", O_WRONLY | O_CREAT);
	if (ret < 0) {
		perror("open");
		goto out;
	}

	ret = spliced_move(ask, out);
	if (ret < 0)
		printf("Can not receive file from socket\n");

	printf("OK\n");
	close(out);
out:
	close(sk);
	return ret;
}
