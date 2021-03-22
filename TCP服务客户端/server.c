#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

void err_handling(char *message);

int main(int argc , char **argv)
{
	int serv_sock;
	int clit_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clit_addr;
	socklen_t clit_addr_size;

	char message[] = "hello world";

	if(argc != 2)
	{
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	serv_sock = socket(AF_INET,SOCK_STREAM,0); //创建服务端套接字
	if (serv_sock == -1)
	{
		err_handling("socket() error");
	}
	memset(&serv_addr,0,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;			//完成套接字分配
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1)
		err_handling("bind() error");

	if(listen(serv_sock,5) == -1)
		err_handling("listen() error");

	clit_addr_size = sizeof(clit_addr);
	//从队列头取连接请求与客户端建立连接
	clit_sock = accept(serv_sock,(struct sockaddr *)&clit_addr,&clit_addr_size);
	if(clit_sock == -1)
		err_handling("accept() error");

	write(clit_sock,message,sizeof(message));//写数据
	close(clit_sock);
	close(serv_sock);

	return 0;
}

void err_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

