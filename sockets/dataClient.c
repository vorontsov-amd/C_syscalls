#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define SOCK_NAME	"mysocket"
/* Преобразовать данную программу, чтобы она периодически проверяла число файлов в некоторой директории
и посылала результат, если число файлов изменилось. */

int getFileCount(const char* path);

int main(int argc, char ** argv) //argv[1] = path
{
	if (argc < 2) {
		fprintf(stderr, "Too few arguments\n");
		return 1;
	}

	int sock = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (sock == -1) {
		fprintf (stderr, "socket() error\n");
		return 1;
	}

	struct sockaddr_un addr = {};
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCK_NAME);
	
    char msg[10] = {};
    while (1) {
        int file_count = getFileCount(argv[1]);
        snprintf(msg, 10, "%d", file_count);

        if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &addr, SUN_LEN(&addr)) == -1) {
            fprintf(stderr, "sendto() error\n");
            return 1;
        }

        sleep(5);
    }

	close(sock);
	return 0;
}

int getFileCount(const char* path) 
{
    int file_count = 0;
    int dir_count = 0;
    int total = 0;

    DIR * dirp;
    struct dirent * entry;

    dirp = opendir(path); 
    while ((entry = readdir(dirp)) != NULL) {
        total++;
        if (entry->d_type == DT_REG) {
            file_count++;
        }
    }

    dir_count = total - file_count;

    closedir(dirp);

    return file_count;
}