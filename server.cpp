#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/stat.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <pthread.h>

#include <mysql.h>

#include <dirent.h>

using namespace std;

#define MAXLINE 1024

#define SERV_PORT 8888

#define LISTENQ 100

#define BUFFER_SIZE 4096

#define FILE_NAME_MAX_SIZE 1024

struct s_info
{

	struct sockaddr_in cliaddr;

	int connfd;
};

void recvFile(char *file_path, int file_size, int socket) // 接收文件函数
{
	FILE *fp;
	fp = fopen(file_path, "wb");
	if (NULL == fp)
	{
		printf("File Can Not Open To Write\n");
		exit(1);
	}

	char buffer[BUFFER_SIZE] = {0};
	int recv_len;
	int write_len;
	int recv_size = 0;

	while (recv_len = recv(socket, buffer, BUFFER_SIZE, 0))
	{
		if (recv_len < 0)
		{
			printf("Recieve Data Failed!\n");
			break;
		}
		printf("recv_len = %d\n", recv_len);

		recv_size += recv_len;

		write_len = fwrite(buffer, sizeof(char), recv_len, fp);

		if (write_len < recv_len)
		{
			printf("File Write Failed!\n");
			break;
		}
		memset(buffer, 0, sizeof(buffer));

		if (recv_size == file_size)
		{
			printf("Recieve File Finished\n");
			break;
		}
	}

	fclose(fp);
}

void sendFile(char * file_path, int socket) // 发送文件函数 , 在发送文件之前，需要获取文件的大小，并传给接收方 , 即file_size 
{ 
	char buffer[BUFFER_SIZE];
	int file_block_length = 0;
	FILE* fp;
	fp = fopen(file_path, "rb");
	if (fp == NULL)
	{
		printf("File Not Found!\n");
	}
	else
	{
		memset(buffer, 0, sizeof(buffer));
		while ((file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
		{
			printf("file_block_length = %d\n", file_block_length);
			//Send the string to the buffer
			if (send(socket, buffer, file_block_length, 0) < 0)
			{
				printf("Send File Failed!\n");
				break;
			}
			memset(buffer,0, sizeof(buffer));
		}
		printf("File:test Transfer Finished!\n");
		fclose(fp);
	}
}


void *dealMsg(void *arg)

{

	int n, i;

	struct s_info *ts = (struct s_info *)arg;

	char buf[MAXLINE]; //在线程自己的用户空间栈开辟的,该线程运行结束的时候,主控线程就不能操作这块内存了

	char str[INET_ADDRSTRLEN]; //INET_ADDRSTRLEN 是宏16个字节

	//在创建线程前设置线程创建属性,设为分离态,效率高

	pthread_detach(pthread_self());

	while (1)

	{

		memset(buf, 0, sizeof(buf));

		n = read(ts->connfd, buf, MAXLINE);

		puts(buf); //char *,存储从客户端传输的字符串

		//连接数据库

		MYSQL *conn_ptr;

		int res;

		conn_ptr = mysql_init(NULL);

		if (!conn_ptr)
		{

			printf("mysql_init failed\n");

			//return EXIT_FAILURE;
		}

		conn_ptr = mysql_real_connect(conn_ptr, "localhost", "root", "yrgwansui", "mail", 3306, NULL, 0); //root 为用户名 123456为密码 test为要连接的database

		if (conn_ptr)

			printf("Connection success\n");

		else

			printf("Connection failed\n");

		char result[100];

		switch (buf[0])

	{	case '0':

		{

			char t_name[100];

			char t_ID[100];

			char t_passwd[100];

			int i, j;

			for (i = 2, j = 0; buf[i] != '|'; i++, j++)

				t_name[j] = buf[i];

			t_name[j] = '\0';

			for (j = 0, i++; buf[i] != '|'; i++, j++)

				t_ID[j] = buf[i];

			t_ID[j] = '\0';

			for (j = 0, i++; buf[i] != '\0'; i++, j++)

				t_passwd[j] = buf[i];

			t_passwd[j] = '\0';

			if (conn_ptr)

			{

				char buf[2048];

				sprintf(buf, "insert into UserTable (ID, name, passwd) value('%s', '%s', '%s');", t_ID, t_name, t_passwd);

				res = mysql_query(conn_ptr, buf); //可以把insert语句替换成delete或者update语句，都一样的

				printf("%d\n", res);

				if (!res)

				{

					result[0] = '1';

					result[1] = '\0';
				}

				else

				{ //打印出错误代码及详细信息

					result[0] = '0';

					result[1] = '\0';

					perror("mysql");
				}

				//printf ("%c\n",result[0]);
			}

			else

			{

				printf("Connection failed\n");
			}

			write(ts->connfd, result, strlen(result));
		}

		break;

			//-------------------------------------------------------------------------------

		case '1': //登录

		{

			int i, j;

			char id[100] = "", pas[100] = "";

			for (i = 2, j = 0; buf[i] != '|'; i++, j++)

				id[j] = buf[i];

			id[j] = '\0';

			for (i++, j = 0; buf[i] != '\0'; i++, j++)

				pas[j] = buf[i];

			pas[j] = '\0';

			if (conn_ptr)

			{

				char buf[2048] = "";

				MYSQL_RES *res_ptr;

				sprintf(buf, "select * from UserTable where ID='%s' and passwd='%s';", id, pas);

				puts(buf);

				res = mysql_query(conn_ptr, buf);

				printf("%d\n", res);

				res_ptr = mysql_store_result(conn_ptr);

				int i = 0;

				MYSQL_ROW row;

				while ((row = mysql_fetch_row(res_ptr)))

				{

					i++;
				}

				printf("%d\n", i);

				if (i != 0)

				{

					result[0] = '1';

					result[1] = '\0';
				}

				else
				{ //打印出错误代码及详细信息

					result[0] = '0';

					result[1] = '\0';

					perror("mysql");
				}

				mysql_free_result(res_ptr);
			}

			else

			{

				printf("Connection failed\n");
			}

			write(ts->connfd, result, strlen(result));
		}

		break;

			//------------------------------------------------------------------------------------------------

		case '2': //write an email

		{

			//2|senderid|receiverid|title|sendtime|0/n|content

			char sid[100], rid[100], tle[100], stime[100], content[4096];

			int att_num; //闄勪欢鏁伴噺

			int i, j;

			for (i = 2, j = 0; buf[i] != '|'; i++, j++)

				sid[j] = buf[i];

			sid[j] = '\0';

			puts(sid);

			for (i++, j = 0; buf[i] != '|'; i++, j++)

				rid[j] = buf[i];

			rid[j] = '\0';

			puts(rid);

			for (i++, j = 0; buf[i] != '|'; i++, j++)

				tle[j] = buf[i];

			tle[j] = '\0';

			puts(tle);

			for (i++, j = 0; buf[i] != '|'; i++, j++)

				stime[j] = buf[i];

			stime[j] = '\0';

			puts(stime);

			i++;

			printf("%c\n", buf[i]);

			att_num = buf[i] - '0';

			printf("%d\n", att_num);

			i += 2;

			for (j = 0; buf[i] != '\0'; i++, j++)

				content[j] = buf[i];

			content[j] = '\0';

			puts(content);

			if (conn_ptr)

			{

				char buf[2048] = "";

				sprintf(buf, "insert into EmailTable (title, content, state, recieverID, sendTime, senderID) value('%s', '%s', 1, '%s', '%s', '%s');", tle, content, rid, stime, sid);

				res = mysql_query(conn_ptr, buf);

				printf("%d\n", res);

				if (!res)

				{ //鎻掑叆鎴愬姛

					MYSQL_RES *ptr;

					sprintf(buf, "select last_insert_id();");

					puts(buf);

					res = mysql_query(conn_ptr, buf);

					printf("%d\n", res);

					ptr = mysql_store_result(conn_ptr); //ptr鎸囧悜缁撴灉闆?						mysql_row row;

					MYSQL_ROW row = mysql_fetch_row(ptr);

					puts(row[0]);

					/*	char eid[100] = "";



						snprintf(eid, sizeof(eid), "%d", eid);  */

					write(ts->connfd, row[0], strlen(row[0])); //杩斿洖emailid

					mysql_free_result(ptr);

					char name[200] = {0};

					char path[1024] = "/home/yrg/server/";

					char e_id[100] = "";

					if (att_num)

					{

						strcpy(e_id, row[0]);

						strcat(path, e_id);

						puts(path);

						mkdir(path, 00700);

						result[0] = '1';

						result[1] = '\0';
					}

					// 有附件

					while (att_num)

					{
						char path_tmp[100] = "";
						strcpy(path_tmp, path);
						printf("num:%d\n", att_num);

						memset(name, 0, sizeof(name));

						// memset(path, 0, sizeof(path));

						int length = recv(ts->connfd, name, FILE_NAME_MAX_SIZE, 0); // 服务器接收文件

						puts(name);

						printf("writing x");

						write(ts->connfd, "x", 1);

						strcat(path_tmp, "/");

						strcat(path_tmp, name);

						printf("recving file  path : %s\n", path_tmp);

						char len[5] = {0};
						int file_size;
						size_t l = recv(ts->connfd, (char *)&file_size, 4, 0);

						printf("File_size:%d\n",file_size);

						recvFile(path_tmp,file_size ,ts->connfd);

						att_num--;

						//write(ts->connfd, result, 2);
					}
				}

				else

				{

					//閹垫挸宓冮崙娲晩鐠囶垯鍞惍浣稿挤鐠囷妇绮忔穱鈩冧紖

					// result[0]= '0';

					// result[1]='\0';

					perror("mysql");
				}

				//printf ("%c\n",result[0]);
			}

			else
			{

				printf("connection failed\n");
			}
		}

		break;
		//------------------------------------------------------------------------------------------
		case '3': //存草稿

		{

			//2|senderid|receiverid|title|sendtime|0/n|content

			char sid[100], rid[100], tle[100], stime[100], content[4096];

			int att_num; //闄勪欢鏁伴噺

			int i, j;

			for (i = 2, j = 0; buf[i] != '|'; i++, j++)

				sid[j] = buf[i];

			sid[j] = '\0';

			puts(sid);

			for (i++, j = 0; buf[i] != '|'; i++, j++)

				rid[j] = buf[i];

			rid[j] = '\0';

			puts(rid);

			for (i++, j = 0; buf[i] != '|'; i++, j++)

				tle[j] = buf[i];

			tle[j] = '\0';

			puts(tle);

			for (i++, j = 0; buf[i] != '|'; i++, j++)

				stime[j] = buf[i];

			stime[j] = '\0';

			puts(stime);

			i++;

			printf("%c\n", buf[i]);

			att_num = buf[i] - '0';

			printf("%d\n", att_num);

			i += 2;

			for (j = 0; buf[i] != '\0'; i++, j++)

				content[j] = buf[i];

			content[j] = '\0';

			puts(content);

			if (conn_ptr)

			{

				char buf[2048] = "";

				sprintf(buf, "insert into EmailTable (title, content, state, recieverID, sendTime, senderID) value('%s', '%s', 3, '%s', '%s', '%s');", tle, content, rid, stime, sid);

				res = mysql_query(conn_ptr, buf);

				printf("%d\n", res);

				if (!res)

				{ //鎻掑叆鎴愬姛

					MYSQL_RES *ptr;

					sprintf(buf, "select last_insert_id();");

					puts(buf);

					res = mysql_query(conn_ptr, buf);

					printf("%d\n", res);

					ptr = mysql_store_result(conn_ptr); //ptr鎸囧悜缁撴灉闆?						mysql_row row;

					MYSQL_ROW row = mysql_fetch_row(ptr);

					puts(row[0]);

					/*	char eid[100] = "";



						snprintf(eid, sizeof(eid), "%d", eid);  */

					write(ts->connfd, row[0], strlen(row[0])); //杩斿洖emailid

					mysql_free_result(ptr);

					char name[200] = {0};

					char path[1024] = "/home/yrg/server/";

					char e_id[100] = "";

					if (att_num)

					{

						strcpy(e_id, row[0]);

						strcat(path, e_id);

						puts(path);

						mkdir(path, 00700);

						result[0] = '1';

						result[1] = '\0';
					}

					// 有附件

					while (att_num)

					{
						char path_tmp[100] = "";
						strcpy(path_tmp, path);
						printf("num:%d\n", att_num);

						memset(name, 0, sizeof(name));

						// memset(path, 0, sizeof(path));

						int length = recv(ts->connfd, name, FILE_NAME_MAX_SIZE, 0); // 服务器接收文件

						puts(name);

						printf("writing x");

						write(ts->connfd, "x", 1);

						strcat(path_tmp, "/");

						strcat(path_tmp, name);

						printf("recving file  path : %s\n", path_tmp);

						char len[5] = {0};
						int file_size;
						size_t l = recv(ts->connfd, (char *)&file_size, 4, 0);

						//recvfile(path, ts->connfd);

						recvFile(path_tmp,file_size ,ts->connfd);

						att_num--;

						//write(ts->connfd, result, 2);
					}
				}

				else

				{

					//閹垫挸宓冮崙娲晩鐠囶垯鍞惍浣稿挤鐠囷妇绮忔穱鈩冧紖

					// result[0]= '0';

					// result[1]='\0';

					perror("mysql");
				}

				//printf ("%c\n",result[0]);
			}

			else
			{

				printf("connection failed\n");
			}
		}

		break;

			//------------------------------------------------------------------------------------------------------------------------------------------

		case '4': //收件箱

		{

			//4|user_ID

			puts(buf);

			char userID[25] = "";

			char buff[200] = "";

			MYSQL_RES *result_mysql;

			for (int i = 0, j = 2; buf[j] != '\0'; i++, j++)

			{

				userID[i] = buf[j];
			}

			puts(userID);

			sprintf(buff, "select * from EmailTable where recieverID = '%s' and (state = 1 or state = 2 ); ", userID);

			res = mysql_query(conn_ptr, buff);

			printf("%d\n", res);

			puts(buff);

			if (!res)

			{

				result_mysql = mysql_store_result(conn_ptr);

				int flag = 0;

				flag = mysql_num_rows(result_mysql);

				printf("%d\n", flag);

				if (flag)

				{

					memset(buff, 0, sizeof(buff));

					//3,1,5,7,0

					//返回 state |title| sendTime|senderID|emailID;

					MYSQL_ROW row;

					while ((row = mysql_fetch_row(result_mysql)))

					{

						strcat(buff, row[3]);

						strcat(buff, "|");

						strcat(buff, row[7]);

						strcat(buff, "|");

						strcat(buff, row[1]);

						strcat(buff, "|");

						strcat(buff, row[5]);

						strcat(buff, "|");

						strcat(buff, row[0]);

						strcat(buff, ";");
					}

					puts(buff);

					mysql_free_result(result_mysql);

					write(ts->connfd, buff, strlen(buff));

					memset(buff, 0, sizeof(buff));
				}

				else

				{

					write(ts->connfd, "no message", 11);
				}
			}

			else

				perror("mysql");
		}

		break;

			//--------------------------------------------------------------------------------------------------------------------

		case '5': //已发送

		{

			char userID[25] = "";

			char buff[200] = "";

			MYSQL_RES *result_mysql;

			for (int i = 0, j = 2; buf[j] != '\0'; i++, j++)

			{

				userID[i] = buf[j];
			}

			puts(userID);

			sprintf(buff, "select * from EmailTable where senderID = '%s' and (state =1 or state =2); ", userID);

			res = mysql_query(conn_ptr, buff);

			printf("%d\n", res);

			puts(buff);

			if (!res)

			{

				result_mysql = mysql_store_result(conn_ptr);

				int flag = 0;

				flag = mysql_num_rows(result_mysql);

				printf("%d\n", flag);

				if (flag)

				{

					memset(buff, 0, sizeof(buff)); //

					MYSQL_ROW row;

					while ((row = mysql_fetch_row(result_mysql)))

					{

						strcat(buff, row[3]);

						strcat(buff, "|");

						strcat(buff, row[7]);

						strcat(buff, "|");

						strcat(buff, row[1]);

						strcat(buff, "|");

						strcat(buff, row[5]);

						strcat(buff, "|");

						strcat(buff, row[0]);

						strcat(buff, ";");
					}

					puts(buff);

					mysql_free_result(result_mysql);

					write(ts->connfd, buff, strlen(buff));
				}

				else

				{

					printf("else case");

					write(ts->connfd, "no message", 20);
				}
			}

			else

				perror("mysql");
		}

		break;

			//--------------------------------------------------------------------------------------------------------------------------------------

		case '6': //已删除

		{

			char userID[25] = "";

			char buff[200] = "";

			MYSQL_RES *result_mysql;

			for (int i = 0, j = 2; buf[j] != '\0'; i++, j++)

			{

				userID[i] = buf[j];
			}

			puts(userID);

			sprintf(buff,

					"select * from EmailTable where (recieverID = '%s' or senderID = '%s') and state = 4; ",

					userID, userID);

			res = mysql_query(conn_ptr, buff);

			printf("%d\n", res);

			puts(buff);

			if (!res)

			{

				result_mysql = mysql_store_result(conn_ptr);

				int flag = 0;

				flag = mysql_num_rows(result_mysql);

				printf("%d\n", flag);

				if (flag)

				{

					memset(buff, 0, sizeof(buff));

					MYSQL_ROW row;

					while ((row = mysql_fetch_row(result_mysql)))

					{

						strcat(buff, row[3]);

						strcat(buff, "|");

						strcat(buff, row[7]);

						strcat(buff, "|");

						strcat(buff, row[1]);

						strcat(buff, "|");

						strcat(buff, row[5]);

						strcat(buff, "|");

						strcat(buff, row[0]);

						strcat(buff, ";");
					}

					puts(buff);

					mysql_free_result(result_mysql);

					write(ts->connfd, buff, strlen(buff));
				}

				else

				{

					write(ts->connfd, "no message", 11);
				}
			}

			else

				perror("mysql");
		}

		break;

			//-------------------------------------------------------------------------------------------------------------------------

		case '7': //草稿箱

		{

			char userID[25] = "";

			char buff[200] = "";

			MYSQL_RES *result_mysql;

			for (int i = 0, j = 2; buf[j] != 0; i++, j++)

			{

				userID[i] = buf[j];
			}

			sprintf(buff, "select * from EmailTable where senderID = '%s' and state = 3;", userID);

			res = mysql_query(conn_ptr, buff);

			printf("%d\n", res);

			puts(buff);

			if (!res)

			{

				result_mysql = mysql_store_result(conn_ptr);

				int flag = 0;

				flag = mysql_num_rows(result_mysql);

				printf("%d\n", flag);

				if (flag)

				{

					memset(buff, 0, sizeof(buff));

					//3,1,5,7,0

					//返回 state |title| sendTime|senderID|emailID;

					MYSQL_ROW row;

					while ((row = mysql_fetch_row(result_mysql)))

					{

						strcat(buff, row[3]);

						strcat(buff, "|");

						strcat(buff, row[7]);

						strcat(buff, "|");

						strcat(buff, row[1]);

						strcat(buff, "|");

						strcat(buff, row[5]);

						strcat(buff, "|");

						strcat(buff, row[0]);

						strcat(buff, ";");
					}

					puts(buff);

					mysql_free_result(result_mysql);

					write(ts->connfd, buff, strlen(buff) + 1);
				}

				else

				{

					printf("noMESSAGE\n");

					write(ts->connfd, "no message", 20);
				}
			}

			else

				perror("mysql");
		}

		break;

			//---------------------------------------------------------------------------------------------------------------------------

		case '8': //彻底删除  8|emailID

		{

			char emailID[25] = "";

			char buff[200] = "";

			MYSQL_RES *result_mysql;

			for (int i = 0, j = 2; buf[j] != 0; i++, j++)

			{

				emailID[i] = buf[j];
			}

			int eid = atoi(emailID);

			sprintf(buff, "delete from EmailTable where emailID = %d;", eid);

			puts(buff);

			res = mysql_query(conn_ptr, buff);

			printf("%d\n", res);

			int flag = 0;

			flag = mysql_affected_rows(conn_ptr);

			if (flag)

				write(ts->connfd, "1", 5);

			else

				write(ts->connfd, "0", 5);
		}

		break;

			//---------------------------------------------------------------------------------------------------------------------------

		case '9': //标记已读  9|emailID

		{

			char emailID[25] = "";

			char buff[200] = "";

			MYSQL_RES *result_mysql;

			for (int i = 0, j = 2; buf[j] != 0; i++, j++)

			{

				emailID[i] = buf[j];
			}

			int eid = atoi(emailID);

			sprintf(buff, "update EmailTable set state = 2 where emailID = %d;", eid);

			res = mysql_query(conn_ptr, buff);

			printf("%d\n", res);

			puts(buff);

			int flag = 0;

			flag = mysql_affected_rows(conn_ptr);

			if (flag)

				write(ts->connfd, "1", 1);

			else

				write(ts->connfd, "0", 1);
		}

		break;

			//------------------------------------------------------------------------------------------------------------------

		case 'a': //删除  a|emailID

		{

			char emailID[25] = "";

			char buff[200] = "";

			MYSQL_RES *result_mysql;

			for (int i = 0, j = 2; buf[j] != 0; i++, j++)

			{

				emailID[i] = buf[j];
			}

			int eid = atoi(emailID);

			sprintf(buff, "update EmailTable set state = 4 where emailID = %d;", eid);

			res = mysql_query(conn_ptr, buff);

			printf("%d\n", res);

			puts(buff);

			int flag = 0;

			flag = mysql_affected_rows(conn_ptr);

			if (flag)

				write(ts->connfd, "1", 1);

			else

				write(ts->connfd, "0", 1);
		}

		break;

			//-------------------------------------------------------------------------------------------------------------

		case 'r': //阅读邮件  r|emailID

		{

			char emailID[25] = "";

			char buff[200] = "";

			char file_name[100] = "";

			MYSQL_RES *result_mysql;

			for (int i = 0, j = 2; buf[j] != 0; i++, j++)

			{

				emailID[i] = buf[j];
			}

			int eid = atoi(emailID);

			printf("emailID=%d\n", eid);

			sprintf(buff, "update EmailTable set state = 2 where emailID = %d;", eid);

			res = mysql_query(conn_ptr, buff);
			memset(buff,0,sizeof(buff));

			sprintf(buff, "select * from EmailTable where emailID = %d;", eid);

			puts(buff);

			res = mysql_query(conn_ptr, buff);

			if (!res)

			{

				result_mysql = mysql_store_result(conn_ptr);

				int flag = 0;

				flag = mysql_num_rows(result_mysql);

				printf("%d\n", flag);

				if (flag)

				{

					memset(buff, 0, sizeof(buff));

					MYSQL_ROW row;

					while ((row = mysql_fetch_row(result_mysql)))

					{

						strcat(buff, row[1]);

						puts(buff);

						strcat(buff, "|");

						puts(buff);

						strcat(buff, row[2]);

						puts(buff);

						strcat(buff, "|");

						puts(buff);

						strcat(buff, row[7]);

						puts(buff);

						strcat(buff, "|");

						puts(buff);

						DIR *dir;

						char path[100] = "/home/yrg/server/";

						strcat(path, emailID);

						puts(path);

						dir = opendir(path);

						if (dir == NULL)

						{

							strcat(buff, "0");

							puts(buff);
						}

						else
						{	
							int num=0;
							struct dirent *ptr;

							while ((ptr = readdir(dir)) != NULL)

							{

								// printf("d_name: %s\n", ptr->d_name);

								char tmp[20] = "";

								strcpy(tmp, ptr->d_name);

								if (strcmp(tmp, ".") == 0 || strcmp(tmp, "..") == 0)

								{

									continue;
								}

								else

								{

									strcat(file_name, tmp);

									strcat(file_name, ";");
									num++;
								}
							}
							char NUM[2]="";
							printf("num =%d",num);
							NUM[0]=num+'0';
							strcat(buff,NUM);
							puts(buff);
							strcat(buff,"|");
							puts(buff);
							strcat(buff, file_name);
							puts(buff);
						}

						closedir(dir);
					}

					puts(buff);

					mysql_free_result(result_mysql);

					write(ts->connfd, buff, strlen(buff));
				}

				else

				{

					write(ts->connfd, "no message", 11);
				}
			}
		}
		break;
		case 'f': //恢复邮件  f|emailID
		{
			char emailID[25] = "";
			char file_name[100] = "";
			char buff[100] = "";
			for (int i = 0, j = 2; buf[j] != 0; i++, j++)
			{
				emailID[i] = buf[j];
			}
			int eid = atoi(emailID);
			printf("e_id=%d\n", eid);
			puts(emailID);
			sprintf(buff, "update EmailTable set state = 2 where emailID = %d;", eid);
			res = mysql_query(conn_ptr, buff);
			printf("res = %d\n", res);
			puts(buff);
			int flag = 0;
			flag = mysql_affected_rows(conn_ptr);
			printf("flag = %d", flag);
			if (flag)
				write(ts->connfd, "1", 1);
			else
				write(ts->connfd, "0", 1);
		}
		break;

		case 'd': //  下载附件   d|emailID|file_name
		{

			char emailID[25] = "";
			char buff[200] = "";
			char file_name[50] = "";
			char file_path[100] = "/home/yrg/server/";
			int i, j;
			for (i = 0, j = 2; buf[j] != '|'; i++, j++)
			{
				emailID[i] = buf[j];
			}
			puts(emailID);
			j++;
			for (i = 0; buf[j] != 0; j++, i++)
			{
				file_name[i] = buf[j];
			}
			
			puts(file_name);
			strcat(file_path, emailID);
			strcat(file_path, "/");
			strcat(file_path, file_name);
			printf("file_path : %s\n", file_path);

			FILE *fp;
			fp = fopen(file_path, "rb");
			fseek(fp, 0, SEEK_END);	   //定位到文件末
			int file_size = ftell(fp); //文件长度
			fclose(fp);
			printf("File_size=			%d\n",file_size);

			send(ts->connfd, (char *)&file_size, 4, 0); // 发送文件大小
			printf("send ending\n");

			//
			//sleep(0.02);
			sendFile(file_path, ts->connfd);
			printf("File was sent.\n");
		}
		break;
		}
			//断开数据库连接。

			mysql_close(conn_ptr);

		if (n == 0)

		{

			printf("the other side has been closed.\n");

			break;
		}

		printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &(*ts).cliaddr.sin_addr, str, sizeof(str)), ntohs((*ts).cliaddr.sin_port));

		//发挥回参数。

		//write(ts->connfd, result, strlen(result));
	}

	close(ts->connfd);
}

int main(void)

{

#pragma region prepare server

	struct sockaddr_in servaddr, cliaddr;

	socklen_t cliaddr_len;

	int listenfd, connfd;

	int i = 0;

	pthread_t tid;

	struct s_info ts[100];

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;

	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	servaddr.sin_port = htons(SERV_PORT);

	int ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	if (ret)

	{

		printf("Server Bind Port : %d Failed!", SERV_PORT);

		exit(1);
	}

	listen(listenfd, LISTENQ);

	printf("Waiting clients ...\n");

#pragma endregion

	while (1)
	{

		cliaddr_len = sizeof(cliaddr);

		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

		ts[i].cliaddr = cliaddr;

		ts[i].connfd = connfd;

		/* 达到线程最大数时，pthread_create出错处理, 增加服务器稳定性 */

		if (i == 100)

		{

			printf("Sever is Full!\n");

			break;
		}

		pthread_create(&tid, NULL, dealMsg, (void *)&ts[i]); //把accept得到的客户端信息传给线程，让线程去和客户端进行数据的收发

		i++;
	}

	return 0;
}
