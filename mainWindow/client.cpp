#include "client.h"
#include "user.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>  // timer


#define BUFFER_SIZE 4096
#define FILE_NAME_MAX_SIZE 512
#define SERV_PORT 8888
#define PATH_MAX_SIZE 1024
#define MAXLINE 1024








bool client::connectToServer()
{

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    qDebug() << "sockfd:" << sockfd;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.43.100", &servaddr.sin_addr.s_addr);//本地ip
    servaddr.sin_port = htons(SERV_PORT);
    //
    int ret = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    qDebug() << "ret:" << ret;

    //连接
    if(ret == -1)
    {
        qDebug()<<"connection failed";
        return 0;
    }


    else
    {
        qDebug()<<" success!";

    }
    return 1;
}


void client::recvFile(QString path, int file_size) // 接收文件函数
{



    qDebug()<<"文件长度为："<< file_size;

    QByteArray ba = path.toLatin1();
    char *file_path = ba.data();
    FILE* fp;

    fp = fopen(file_path, "wb");
    if (NULL == fp)
    {
        qDebug()<<"File Can Not Open To Write";
        exit(1);
    }

    char buffer[BUFFER_SIZE] = {0};
    int recv_len;
    int write_len;
    int recv_size = 0;

    while(recv_len = recv(sockfd, buffer, BUFFER_SIZE,0))
    {
        if(recv_len < 0)
        {
            qDebug()<<"Recieve Data Failed!";
            break;
        }
        qDebug()<<"recv_len = "<< recv_len;

        recv_size += recv_len;

        write_len = fwrite(buffer, sizeof(char), recv_len, fp);

        if(write_len < recv_len)
        {
            qDebug()<<"File Write Failed!\n";
            break;
        }
        memset(buffer,0,sizeof(buffer));

        if(recv_size == file_size)
        {
            qDebug()<<"Recieve File Finished";
            break;
        }
    }

    fclose(fp);
}

void client::sendFile(QString path) // 发送文件函数
{
    QByteArray ba = path.toLatin1();
    char *file_path = ba.data();
    FILE* fp;
    fp = fopen(file_path, "rb");
    fseek(fp, 0, SEEK_END); //定位到文件末
    int file_size = ftell(fp); //文件长度
    fseek(fp,0,SEEK_SET);
    qDebug() << "file_size" << file_size;
    send(sockfd, (char*)&file_size, 4, 0); // 发送文件大小
    qDebug() << "sent file_size";
    char buffer[BUFFER_SIZE];
    int file_block_length = 0;

    if (fp == NULL)
    {
        qDebug()<<"File Not Found!";
    }
    else
    {
        memset(buffer, 0, sizeof(buffer));
        while ((file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
        {
            qDebug()<<"file_block_length = "<< file_block_length;
            //Send the string to the buffer
            if (send(sockfd, buffer, file_block_length, 0) < 0)
            {
                qDebug()<<"Send File Failed!\n";
                break;
            }
            memset(buffer, 0, sizeof(buffer));
        }
        qDebug()<<"File:test Transfer Finished!";
        fclose(fp);
    }
}

QString client::sendMessage(QString qstr){
    QByteArray ba=qstr.toLatin1();
    char *c=ba.data();
    qDebug() << "sockfd:" << sockfd;

    char buf[MAXLINE] = "";

    strcat(buf, c);
    qDebug()<<"buf"<<buf;

    qDebug() << "send begin";
    write(sockfd, buf, strlen(buf));
    //发送

    char buf_2[MAXLINE] = "";
    int n = read(sockfd, buf_2, MAXLINE);
    qDebug() << buf_2 << "n" << n;
    QString str = QString(buf_2);
    qDebug() <<"received:" << str;
    return str;

}

int client::sendMessage2(QString qstr){
    QByteArray ba=qstr.toLatin1();
    char *c=ba.data();
    qDebug() << "sockfd:" << sockfd;

    char buf[MAXLINE] = "";

    strcat(buf, c);
    qDebug()<<"buf"<<buf;

    qDebug() << "send begin";
    write(sockfd, buf, strlen(buf));
    //发送

    char buf_2[MAXLINE] = "";
    int file_size;
    size_t l = recv(sockfd, (char*)&file_size, 4, 0);

    qDebug() << file_size;
    return file_size;

}




void client::closeConnection(){
    close(sockfd);
}

client::client()
{

}

