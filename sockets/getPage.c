#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define BUF_LEN		4096
#define HTTP_PORT	80

/* Получить и распечатать какую либо страничку из интернета man 3 gethostbyname */
int main(int argc, char ** argv)
{	
	if (argc < 2) {
		fprintf(stderr, "Too few arguments\n");
		return 1;
	}

	char* buf = (char*)malloc(BUF_LEN);
	if (buf == NULL) {
		fprintf(stderr, "malloc() error\n");
		return 1;
	}

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		fprintf (stderr, "socket() error\n");
		return 1;
	}

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;

	struct hostent* host = NULL;
	host = gethostbyname(argv[1]);
	if (host == NULL) {
		fprintf(stderr, "Unknown server\n");
		return 1;
	}

	addr.sin_addr = *(struct in_addr*) host->h_addr_list[0];
	addr.sin_port = HTTP_PORT;

	if (connect(sock, (struct sockaddr*) &addr, sizeof (addr)) == -1) {
		fprintf(stderr, "connect() error\n");
		return 1;
	}
	
	strcpy(buf, "GET /");
    strcat(buf, " HTTP/1.1\nHost: ");
    strcat(buf, argv[1]);
    strcat(buf, "\n\n");
	
	write(sock, buf, strlen(buf));
	
	int count = 0;
	while ((count = read(sock, buf, BUF_LEN)) > 0) {
		write(1, buf, count);
    }

	close(sock);
	free(buf);
	return 0;
}