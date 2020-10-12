#ifndef CLIENT_H
#define CLIENT_H
#include <QString>
/* client.c */
#include "user.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <QDebug>


class client
{
public:
    client();
    bool connectToServer();//1 success 0 fail
    QString sendMessage(QString);
    void sendFile(QString path);
    void closeConnection();
    void* print_message(QString);
    void recvFile(QString path, int file_size);
    int sendMessage2(QString qstr);

private:
    struct sockaddr_in servaddr;


};

#endif // CLIENT_H
