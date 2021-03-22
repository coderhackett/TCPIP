#include <stdio.h>
#include <unistd.h>

int gval = 10;
int main(int argc, char const *argv[])
{
	pid_t pid;
	int lval = 20;

	gval++;
	lval+=5;
	pid = fork();	//创建父子进程

	if (pid == 0)	//子进程
	{
		gval += 2;
		lval += 2;
	}
	else			//父进程
	{
		gval -= 2;
		lval -= 2;
	}
	if (pid == 0)
	{
		printf("child proc: gval = %d,lval = %d\n", gval,lval);
	}else
	{
		printf("parent proc: gval = %d,lval = %d\n", gval,lval);
	}
	return 0;
}