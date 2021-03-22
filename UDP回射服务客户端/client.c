#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#define BUFF_SIZE 1024

void err_handling(char *message);

int main(int argc, char const *argv[])
{
	int sock;
	struct sockaddr_in serv_addr,from_addr;	
	char message[BUFF_SIZE];
	int str_len;
	socklen_t adr_sz;

	if(argc != 3)
	{
		printf("Usage : %s <IP> <port> \n",argv[0]);
		exit(1);
	}

	sock = socket(PF_INET,SOCK_DGRAM,0);
	if(sock == -1)
		err_handling("socket() error");

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	//第二种，连接服务端
	connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

	while(1)
	{
		fputs("input message(Q to quit):",stdout);
		fgets(message,BUFF_SIZE,stdin);
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
		{
			break;
		}
		//第一种
		/*sendto(sock,message,strlen(message),0,
			(struct sockaddr *)&serv_addr,sizeof(serv_addr));	//客户端发送数据*

		adr_sz = sizeof(from_addr);								//客户端接收服务端的信息
		str_len = recvfrom(sock,message,BUFF_SIZE,0,
			(struct sockaddr *)&from_addr,&adr_sz);*/

		//第二种,需要先connect服务端
		write(sock,message,strlen(message));

		str_len = read(sock,message,sizeof(message) -1);

		message[str_len] = 0;
		printf("message from server: %s\n", message);
	}

	close(sock);

	return 0;
}


void err_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
