#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>

#include "spliced_move.h"

int spliced_move(int fd_in, int fd_out, long long size)
{
	int ret;
	int p[2];
	struct stat st;
	long long len;

	ret = pipe(p);
	if (ret < 0) {
		perror("pipe");
		goto exit;
	}

	ret = fstat(fd_in, &st);
	if (ret < 0) {
		perror("fstat");
		goto close;
	}

	if ((st.st_mode & S_IFMT) == S_IFSOCK) {
		len = 33554431;
	} else {
		len = st.st_size;
	}
	//if (size > 0 && size < len)
	if (size > 0)
		len = size;
	//printf("Input len = %d|%d|%Ld\n", len, st.st_size, size);

	while(len > 0) {
		int chunk = __IO_BUFSIZE;
		if (chunk > len)
			chunk = len;

		ret = chunk = splice(fd_in, NULL, p[1], NULL, chunk, SPLICE_F_MOVE | SPLICE_F_MORE);
		if (ret < 0) {
			perror("splice to pipe");
			goto close;
		}
		len -= chunk;

		while (chunk > 0) {
			ret = splice(p[0], NULL, fd_out, NULL, chunk, SPLICE_F_MOVE | SPLICE_F_MORE);
			if (ret < 0) {
				perror("splice from pipe");
				goto close;
			}
			chunk -= ret;
		}
	}
close:
	close(p[0]);
	close(p[1]);
exit:
	return ret;
}
