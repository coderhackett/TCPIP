#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUFF_SIZE	4
#define EPOLL_SIZE	50

void setnoblockingmode(int fd);
void err_handling(char *buf);

int main(int argc, char const *argv[])
{
	int serv_sock,clit_sock;
	int str_len = 0, i;

	char buf[BUFF_SIZE];

	struct sockaddr_in serv_addr;
	struct sockaddr_in clit_addr;
	socklen_t clit_addr_size;

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd , event_cnt;

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

	epfd = epoll_create(EPOLL_SIZE);
	ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
	setnoblockingmode(serv_sock);
	event.events = EPOLLIN;
	event.data.fd = serv_sock;
	epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);

	while(1)
	{
		event_cnt = epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);
		if(event_cnt == -1)
		{
			puts("epoll_wait error\n");
			break;
		}
		puts("return epoll_wait\n");
		for (int i = 0; i < event_cnt; i++)
		{
			if(ep_events[i].data.fd == serv_sock)
			{
				clit_addr_size = sizeof(clit_addr);
				clit_sock = accept(serv_sock,(struct sockaddr *)&clit_addr,&clit_addr_size);
				setnoblockingmode(clit_sock);
				event.events = EPOLLIN|EPOLLET;
				event.data.fd = clit_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD,clit_sock,&event);
				printf("connect client: %d\n", clit_sock);
			}
			else
			{
				while(1)
				{
					str_len = read(ep_events[i].data.fd,buf,BUFF_SIZE);
					if(str_len == 0)//close request
					{
						epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
						close(ep_events[i].data.fd);
						printf("close client: %d\n", ep_events[i].data.fd);
						break;
					}
					else if(str_len < 0)
					{
						if(errno == EAGAIN)
							break;
					}
					else
					{
						write(ep_events[i].data.fd,buf,str_len);
					}
				}
			}
		}
	}
	close(serv_sock);
	close(epfd);
	return 0;
}

void setnoblockingmode(int fd)
{
	int flag = fcntl(fd,F_GETFL,0);
	fcntl(fd,F_SETFL,flag | O_NONBLOCK);
}


void err_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
