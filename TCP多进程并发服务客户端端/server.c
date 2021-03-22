#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#define BUFF_SIZE 1024

void err_handling(char *message);
void read_childproc(int sig);

int main(int argc , char **argv)
{
	int serv_sock,clit_sock;
	int str_len = 0, i,state;
	pid_t pid;

	char message[BUFF_SIZE];

	struct sigaction act;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clit_addr;
	socklen_t clit_addr_size;


	if(argc != 2)
	{
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	act.sa_handler = read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	state = sigaction(SIGCHLD,&act,0);

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

	while(1)
	{
		clit_addr_size = sizeof(clit_addr);

		//从队列头取连接请求与客户端建立连接
		clit_sock = accept(serv_sock,(struct sockaddr *)&clit_addr,&clit_addr_size);
		if(clit_sock == -1)
			err_handling("accept() error");
		else
			printf("connect client %d\n", i+1);

		pid = fork();
		if(pid == -1)
		{
			close(clit_sock);
			continue;
		}
		if(pid == 0)	//子进程
		{
			close(serv_sock);
			while((str_len = read(clit_sock,message,BUFF_SIZE)) != 0)
				write(clit_sock,message,str_len);	//读取客户端发送的数据回射
			close(clit_sock);
			puts("client disconnect...\n");
			return 0;
		}
		else
		{
			close(clit_sock);
		}

	}	
	close(serv_sock);

	return 0;
}

void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid = waitpid(-1,&status,WNOHANG);
	printf("remove proc id : %d\n", pid);
}

void err_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

