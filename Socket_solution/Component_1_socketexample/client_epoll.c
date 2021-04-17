/******* client.c ************/

#include <stdlib.h>

#include <stdio.h>

#include <errno.h>

#include <string.h>

#include <netdb.h>

#include <sys/types.h>

#include <netinet/in.h>

#include <sys/socket.h>

int main()

{

	int sockfd;

	struct sockaddr_in server_addr;

	struct hostent* host;

	host = gethostbyname("localhost");



	int portnumber = 1680;



	/* 客户程序开始建立 sockfd 描述符 */

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)

	{

		fprintf(stderr, "Socket Error：%s\a\n", strerror(errno));

		exit(1);

	}

	/* 客户程序填充服务端的资料 */

	bzero(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;

	server_addr.sin_port = htons(portnumber);

	server_addr.sin_addr = *((struct in_addr*)host->h_addr);

	/* 客户程序发起连接请求 */

	if (connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1)

	{

		fprintf(stderr, "Connect Error：%s\a\n", strerror(errno));

		exit(1);

	}





	char buff_up_char[50] = "NMSL";

	if (write(sockfd, buff_up_char, strlen(buff_up_char)) == -1)

	{

		fprintf(stderr, "write Error：%s\n", strerror(errno));

		exit(1);

	}



	close(sockfd);

}

