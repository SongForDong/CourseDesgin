#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "../include/network.h"
#include "../include/ftp_operator.h"
#include <errno.h>


NetWork* nw;
NetWork* data_nw;

char buf[256];


int main(int argc,char* argv[])
{	

	char c_ip[40] = {};
	if(argv[1] == NULL){
		printf("[ERROR] please input IP addr\n");
		return -1;
	}
	strcpy(c_ip,argv[1]);

	nw = open_network('c',SOCK_STREAM,c_ip,21);
	printf("[Info] ip:%s    port:%d\n", c_ip, 21);
	if(NULL == nw)
	{
		printf("open network socket null!\n");
		return -1;
	}

	printf("[Info] Connected to %s.\n",c_ip);

	nrecv(nw,buf,sizeof(buf));
	printf("%s",buf);//220

	for(;;)
	{
		char user[20] = {};
		printf("Name (%s:song):",c_ip);
		gets(user);

		sprintf(buf,"USER %s\n",user);
		printf("[TEST] buf: %s\b", buf);
		nsend(nw, buf, strlen(buf));

		bzero(buf,sizeof(buf));
		nrecv(nw,buf,sizeof(buf));
		printf("[TEST] buf:%s\n",buf);//331

		char pw[20] = {};
		printf("Password:");
		
		struct termios old, new; 
		tcgetattr(0, &old);  // 获取终端属性
		new = old;	
		new.c_lflag &= ~(ECHO | ICANON);// 不使用标准的输出，不显示字符。 
 		tcsetattr(0, TCSANOW, &new);// 设置终端新的属性
		gets(pw);								
		tcsetattr(0, TCSANOW, &old);

		sprintf(buf,"PASS %s\n",pw);
		nsend(nw,buf,strlen(buf));//pw

		bzero(buf,sizeof(buf));
		nrecv(nw,buf,sizeof(buf));
		printf("\n[Info] %s",buf);//230
		if(strstr(buf,"530") == NULL)
		{
			break;
		}
	}

	printf("[Info] Remote system type is UNIX.\n");
	printf("[Info] Using binary mode to transfer files.\n");
	
	ex();
	must();
	char cmd[40] = {};
	while(1)
	{

		printf("mftp> ");
		gets(cmd);
		printf("[TEST] cmd=%s\n", cmd);
		if(strcmp(cmd,"bye")==0)
		{
			break;
		}
		else
		if(strcmp(cmd,"ls")==0)
		{
			printf("[TEST] GO func ls\n");
			printf("[TEST] AFTER GET buf=%s\n", buf);
			ls();
		}

		// 解析指令
		char *cmd1 = malloc(20);
		char *path = malloc(100);
		sscanf(cmd,"%s %s",cmd1,path);
		printf("cmd:%s  path:%s\n", cmd1, path);
		if(strcmp(cmd1,"cd") == 0)
		{
			cd_to(path);
		}
		else
		if(strcmp(cmd1,"get") == 0)
		{
			download(path);
		}
		else
		if(strcmp(cmd1,"put") == 0)
		{
			upload(path);
		}
		else
		if(strcmp(cmd1, "pwd") == 0)
		{
			//printf("Not implement yet\n");
			PWD();
		}
		else
		if(strcmp(cmd1, "rm") == 0){
			RM(path);
		}
		else
		if(strcmp(cmd1, "mkdir") == 0){
			// TODO:
			MKDIR(path);
		}
		else
		if(strcmp(cmd1, "rmd") == 0){
			RMD(path);
		}
		
		//must();
	}

	close_network(nw);
	printf("[Info] 221 Goodbye.\n");//221
	//printf("Segmentation fault? Fuck u!\n");


}

