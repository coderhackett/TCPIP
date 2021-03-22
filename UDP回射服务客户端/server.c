#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#define BUFF_SIZE 1024

void err_handling(char *message);

int main(int argc , char **argv)
{
	int serv_sock,clit_sock;
	int str_len;

	char message[BUFF_SIZE];

	struct sockaddr_in serv_addr;
	struct sockaddr_in clit_addr;
	socklen_t clit_addr_size;


	if(argc != 2)
	{
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET,SOCK_DGRAM,0); //创建服务端套接字
	if (serv_sock == -1)
	{
		err_handling("socket() error");
	}
	memset(&serv_addr,0,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;			//完成套接字分配
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	//绑定
	if (bind(serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1)
		err_handling("bind() error");

	while(1)
	{
		clit_addr_size = sizeof(clit_addr);
		str_len = recvfrom(serv_sock,message,BUFF_SIZE,0,
			(struct sockaddr *)&clit_addr,&clit_addr_size);
		sendto(serv_sock,message,str_len,0,
			(struct sockaddr *)& clit_addr,clit_addr_size);//回射客户的发送过来的信息
	}


	close(serv_sock);

	return 0;
}

void err_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

