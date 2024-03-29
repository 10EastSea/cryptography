#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <json-c/json.h>

static struct sockaddr_in client_addr;
static int client_fd, n, n2, state = 1;
static char recv_data[6000];

int main(int argc, char *argv[])
{
	char *IP = argv[1];
	in_port_t PORT = atoi(argv[2]);

	if (argc != 3)
	{
		printf("Useage : ./client [IP] [PORT]\n");
		exit(0);
	}

	client_fd = socket(PF_INET, SOCK_STREAM, 0);

	client_addr.sin_addr.s_addr = inet_addr(IP);
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);

	if (connect(client_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1)
	{
		printf("Can't Connect\n");
		close(client_fd);
		return -1;
	}

	while (1)
	{

		printf("\nSend Message : ");
		gets(chat_data);

		printf("Message Receives ...\n");

		printf("Receive Message : %s", chat_data);
	}

	close(client_fd);
	return 0;
}
