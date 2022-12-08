#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>

#define QUEUE_LENGTH	10
#define BUF_LEN		    256
#define SOCK_NAME	    "mysocket"

/* Преобразовать данную программу, чтобы она периодически печатала "изменений нет", если их в действительности нет. */
int main()
{
	int sock = socket (PF_UNIX, SOCK_DGRAM, 0);
	if (sock == -1) {
		fprintf (stderr, "socket() error\n");
		return 1;
	}

	char * buf      = (char *)malloc(BUF_LEN);
	char * last_buf = (char *)malloc(BUF_LEN);
    
    	if (buf == NULL || last_buf == NULL) {
		fprintf (stderr, "malloc() error\n");
		return 1;
	}

	struct sockaddr_un saddr = {};
	saddr.sun_family = AF_UNIX;
	strcpy(saddr.sun_path, SOCK_NAME);

	if (bind(sock, (struct sockaddr *) &saddr, SUN_LEN(&saddr)) == -1) {
		fprintf (stderr, "bind() error\n");
		return 1;
	}

	int count = 0;
	while (1) {
		if ((count = recvfrom(sock, buf, BUF_LEN-1, 0, NULL, NULL)) == -1) {
			fprintf(stderr, "recvfrom() error\n");
			return 1;
		}

		buf[count] = '\0';
		
        if (strcmp(last_buf, buf) == 0) {
            puts("no changes");
        }
        else {
            printf("file count = %s\n", buf);
            strncpy(last_buf, buf, BUF_LEN);
        }

		if (!strcmp(buf, "exit")) break;
	}

	free(buf);
	close(sock);
	unlink(SOCK_NAME);
	return 0;
}
