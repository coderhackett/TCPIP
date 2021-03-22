#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define BUFF_SIZE	1024
void err_handling(char *message);

int main(int argc, char const *argv[])
{
	int serv_sock,clit_sock;
	struct sockaddr_in serv_addr,clit_addr;

	struct timeval timeout;
	fd_set reads,cpy_reads;

	socklen_t addr_size;

	int fd_max,str_len,fd_num,i;
	char buf[BUFF_SIZE];

	if(argc != 2)
	{
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET,SOCK_STREAM,0); //创建服务端套接字
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

	FD_ZERO(&reads);	//初始化字符集
	FD_SET(serv_sock,&reads);
	fd_max = serv_sock;

	while(1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;	//设置定时
		timeout.tv_usec = 0;

		if(fd_num = select(fd_max+1,&cpy_reads,0,0,&timeout) == -1)//监听文件描述符
			break;
		if(fd_num == 0)
			continue;
		for (int i = 0; i < fd_max+1; i++)
		{
			if(FD_ISSET(i,&cpy_reads))
			{
				if(i == serv_sock)	//连接响应
				{
					addr_size = sizeof(clit_addr);
					clit_sock = accept(serv_sock,(struct sockaddr*)&clit_addr,&addr_size);
					FD_SET(clit_sock,&reads);
					if(fd_max < clit_sock)
						fd_max = clit_sock;
					printf("connect client: %d\n", clit_sock);
				}
				else	//读取信息
				{
					str_len = read(i,buf,BUFF_SIZE);
					if(str_len == 0)	//关闭连接
					{
						FD_CLR(i,&reads);
						close(i);
						printf("close client: %d\n", i);
					}
					else	//发送数据给对端
					{
						write(i,buf,str_len);
					}
				}

			}
		}
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


