#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <json-c/json.h>

static struct sockaddr_in server_addr, client_addr;
static int server_fd, client_fd, n, n2;
static char recv_data[6000];

int main(int argc, char *argv[])
{
	int len;
	char temp[20];

	if (argc != 2)
	{
		printf("Usage:%s <port>\n", argv[0]);
		exit(1);
	}

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Server can not open socket\n");
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;

	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("Server can not bind local address\n");
		exit(0);
	}

	if (listen(server_fd, 5) < 0)
	{
		printf("Server can not listen connect\n");
		exit(0);
	}

	memset(recv_data, 0, sizeof(recv_data));
	len = sizeof(client_addr);
	printf("===[PORT] : 4000=====\n");
	printf("Server waiting connection request\n");



	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&len);

	if (client_fd < 0)
	{
		printf("Server accept failed\n");
		exit(0);
	}

	inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp, sizeof(temp));
	printf("%s client connect\n", temp);

	printf("\n%s(%d) entered\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	while (1)
	{

		printf("Message Receives ...\n");
		

		printf("Receive Message : %s", chat_data);
		printf("\nSend Message : ");
		gets(chat_data);
		
	}
	close(client_fd);

	close(server_fd);

	return 0;
}