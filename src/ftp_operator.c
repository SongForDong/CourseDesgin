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
#include <errno.h>
#include "../include/ftp_operator.h"


extern NetWork* nw;
extern NetWork* data_nw;

extern char buf[256];



void ex(void)
{
	sprintf(buf,"SYST\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	//puts(buf);

	sprintf(buf,"OPTS UTF8 ON\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	//puts(buf);
	
}

void must(void)
{
	sprintf(buf,"PWD\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	//puts(buf);//257

	sprintf(buf,"PASV\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	puts(buf);//227

	unsigned char ip1,ip2,ip3,ip4,port1,port2;
	sscanf(strchr(buf,'(')+1,"%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",&ip1,&ip2,&ip3,&ip4,&port1,&port2);
	sprintf(buf,"%hhu.%hhu.%hhu.%hhu",ip1,ip2,ip3,ip4);
	

	NetWork* data_nw = open_network('c',SOCK_STREAM,buf,port1*256+port2);
	//printf("connect success fd = %d\n",data_nw->fd);
	
	sprintf(buf,"LIST -al\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	//printf("%s",buf);//150
	
	int ret = 0;
	bzero(buf,sizeof(buf));
	while(ret = nrecv(data_nw,buf,sizeof(buf)))
	{
		//printf("%s",buf);
		bzero(buf,sizeof(buf));
	}
	close_network(data_nw);

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	//printf("%s",buf);//226
}

void ls(void)
{
	bzero(buf, sizeof(buf));
	sprintf(buf,"PWD\n");
	nsend(nw,buf,strlen(buf));
	printf("[TEST] test point1: buf=%s\n", buf);

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	//puts(buf);//257
	printf("[TEST] test point2: buf=%s\n", buf);

	bzero(buf, sizeof(buf));
	sprintf(buf,"PASV\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	//puts(buf);//227
	
	printf("[TEST] test point3: buf=%s\n", buf);

	unsigned char ip1,ip2,ip3,ip4,port1,port2;
	sscanf(strchr(buf,'(')+1,"%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",&ip1,&ip2,&ip3,&ip4,&port1,&port2);
	sprintf(buf,"%hhu.%hhu.%hhu.%hhu",ip1,ip2,ip3,ip4);
	printf("[Info] buf(ip): %s\n", buf);

	NetWork* data_nw = open_network('c',SOCK_STREAM,buf,port1*256+port2);
	printf("connect fd = %d\n",data_nw->fd);
	
	sprintf(buf,"LIST -al\n");
	nsend(nw,buf,strlen(buf));

	printf("[Info] 200 PORT command successful. Consider using PASV.\n");

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	printf("%s",buf);//150
	
	int ret = 0;
	bzero(buf,sizeof(buf));
	while(ret = nrecv(data_nw,buf,sizeof(buf)))
	{
		//printf("\n----------------\n");
		printf("%s",buf);
		bzero(buf,sizeof(buf));
	}
	close_network(data_nw);

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	printf("%s",buf);//226
}

void cd_to(char* cd)
{
	char *dir = cd;
	if(strcmp(dir,"..")==0)
	{
		sprintf(buf,"CDUP %s\n",dir);
	}
	else
	{
		sprintf(buf,"CWD %s\n",dir);
	}
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	printf("[Info] %s",buf);//250
}

void download(char* get)
{
	printf("Download\n");
	char *filename = get;
	sprintf(buf,"TYPE A\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	puts(buf);

	sprintf(buf,"SIZE %s\n",filename);
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	//printf("test for file size error\n");
	puts(buf);

	sprintf(buf,"MDTM %s\n",filename);
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	puts(buf);

	sprintf(buf,"PASV\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	puts(buf);
	
	unsigned char ip1,ip2,ip3,ip4,port1,port2;
	sscanf(strchr(buf,'(')+1,"%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",&ip1,&ip2,&ip3,&ip4,&port1,&port2);
	sprintf(buf,"%hhu.%hhu.%hhu.%hhu",ip1,ip2,ip3,ip4);

	data_nw = open_network('c',SOCK_STREAM,buf,port1*256+port2);
	printf("[Info] Connect success fd = %d\n",data_nw->fd);

	
	sprintf(buf,"RETR %s\n",filename);
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	//printf("test for failed to open file\n");
	puts(buf);

	//strcat("../file4tran/", filename);
	char* targetname;
	sprintf(targetname, "../file4tran/%s", filename);
	printf("%s\n", filename);
	int fd = open(targetname,O_WRONLY|O_CREAT|O_TRUNC,0644);
	//printf("HERE FOR OPEN TEST BELOW test for failed to open file:1\n");
	printf("[TEST] fd=%d\n", fd);
	if(fd < 0)
	{
		perror("open");
		return;
	}
	//printf("HERE FOR OPEN TEST BELOW test for failed to open file:2\n");
	int ret = 0;
	while(ret = nrecv(data_nw,buf,sizeof(buf)))
	{
		//printf("\n[TEST] buf=%s ret=%d\n", buf, ret);
		write(fd,buf,ret);
	}

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	puts(buf);

	close(fd);
	close_network(data_nw);
}

void upload(char* put){
	char* filename = put;
	int file_size = get_filesize(filename);
	if (file_size < 0){
		perror("Upload file:");
		return;
	}
	sprintf(buf, "TYPE A\n");
	nsend(nw, buf, strlen(buf));

	bzero(buf, sizeof(buf));
	nrecv(nw, buf, sizeof(buf));
	puts(buf);

	sprintf(buf,"PASV\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	puts(buf);

	//Get ip addr for data_connnect
	unsigned char ip1,ip2,ip3,ip4,port1,port2;
	sscanf(strchr(buf,'(')+1,"%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",&ip1,&ip2,&ip3,&ip4,&port1,&port2);
	sprintf(buf,"%hhu.%hhu.%hhu.%hhu",ip1,ip2,ip3,ip4);
	printf("%s\n", buf);

	data_nw = open_network('c', SOCK_STREAM, buf, port1*256 + port2);
	

	sprintf(buf, "STOR %s\n", filename);
	nsend(nw, buf, strlen(buf));

	bzero(buf, sizeof(buf));
	nrecv(nw, buf, sizeof(buf));
	puts(buf);

	int fd = open(filename,O_RDONLY, 0644);
	if (fd < 0){
		perror("open");
		return;
	}
	
	int ret = 0;
	bzero(buf, sizeof(buf));
	while (read(fd, buf, 1)){
		nsend(data_nw, buf, strlen(buf));
		bzero(buf, sizeof(buf));
	}

	close_network(data_nw);

	bzero(buf, sizeof(buf));
	nrecv(nw, buf, sizeof(buf));
	puts(buf);

	sprintf(buf, "MDTM %s\n", filename);
	nsend(nw, buf, strlen(buf));

	bzero(buf, sizeof(buf));
	nrecv(nw, buf, sizeof(buf));
	puts(buf);

}

void PWD(){
	
	sprintf(buf,"PWD\n");
	nsend(nw,buf,strlen(buf));

	bzero(buf,sizeof(buf));
	nrecv(nw,buf,sizeof(buf));
	puts(buf);
}

void RM(char *path){
	char* filename = path;
	sprintf(buf, "DELE %s\n", filename);
	nsend(nw, buf, strlen(buf));

	bzero(buf, sizeof(buf));
	nrecv(nw, buf, sizeof(buf));
	puts(buf);
}

void MKDIR(char* path){
	char* filename = path;
	sprintf(buf, "MKD %s\n", filename);
	nsend(nw, buf, strlen(buf));

	bzero(buf, sizeof(buf));
	nrecv(nw, buf, sizeof(buf));
	puts(buf);
}

void RMD(char* path){
	// TODO: implement rmdir
	char* filename = path;
	sprintf(buf, "RMD %s\n", filename);
	nsend(nw, buf, strlen(buf));

	bzero(buf, sizeof(buf));
	nrecv(nw, buf, sizeof(buf));
	puts(buf);
}

off_t get_filesize(char* filename){
    int ret;
	int fd = -1;
	struct stat file_stat;

	fd = open(filename, O_RDONLY);	// 打开文件
	if (fd == -1) {
		printf("Open file %s failed : %s\n", filename, strerror(errno));
		return -1;
	}
	ret = fstat(fd, &file_stat);	// 获取文件状态
	if (ret == -1) {
		printf("Get file %s stat failed:%s\n", filename, strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);
	return file_stat.st_size;
}