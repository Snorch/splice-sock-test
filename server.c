#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>

#include "spliced_move.h"

#define SERV_PORT 20134

void diff_time(struct timeval *t2, struct timeval *t1)
{
	if (t2->tv_usec > t1->tv_usec) {
		t2->tv_usec -= t1->tv_usec;
		t2->tv_sec -= t1->tv_sec;
	} else {
		t2->tv_usec += 1000000 - t1->tv_usec;
		t2->tv_sec -= t1->tv_sec + 1;
	}
}

static void get_sockaddr_in(struct sockaddr_in *addr)
{
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
	long long size = 0;
	struct timeval t1, t2;

	if (argc >= 2)
		size = strtoll(argv[1], NULL, 10);

	//printf("Starting server on port %d\n", SERV_PORT);

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

	//printf("Accepted connection\n");

	ret = out = open("tmpfs/out", O_WRONLY | O_CREAT);
	if (ret < 0) {
		perror("open");
		goto out;
	}

	ret = gettimeofday(&t1, NULL);
	if (ret < 0) {
		perror("gettimeofday");
		goto out;
	}

	ret = spliced_move(ask, out, size);
	if (ret < 0)
		printf("Can not receive file from socket\n");

	ret = gettimeofday(&t2, NULL);
	if (ret < 0) {
		perror("gettimeofday2");
		goto out;
	}

	diff_time(&t2, &t1);

	printf("OK %d.%06d %Ld\n", t2.tv_sec, t2.tv_usec, size);
	close(out);
out:
	close(sk);
	return ret;
}
