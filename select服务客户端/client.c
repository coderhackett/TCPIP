#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#define BUFF_SIZE 1024

void err_handling(char *message);
void write_route(int sock,char *message);
void read_route(int sock,char *message);

int main(int argc, char const *argv[])
{
	int sock;
	pid_t pid;
	struct sockaddr_in serv_addr;	
	char message[BUFF_SIZE];
	int str_len,recv_len,recv_cnt;

	if(argc != 3)
	{
		printf("Usage : %s <IP> <port> \n",argv[0]);
		exit(1);
	}

	sock = socket(PF_INET,SOCK_STREAM,0);
	if(sock == -1)
		err_handling("socket() error");

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1)
		err_handling("connect() error");
	else
		printf("connected \n");

	pid = fork();
	if(pid == 0)
		write_route(sock,message);
	else
		read_route(sock,message);

	close(sock);
	return 0;
}

void read_route(int sock,char *message)
{
	while(1)
	{
		int str_len = read(sock,message,BUFF_SIZE);
		if(str_len == 0)
			return;
		message[BUFF_SIZE] = 0;
		printf("message from server: %s\n", message);
	}
}
void write_route(int sock,char *message)
{
	while(1)
	{
		fgets(message,BUFF_SIZE,stdin);
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
		{
			shutdown(sock,SHUT_WR);
			break;
		}
		write(sock,message,strlen(message));
	}
}

void err_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
