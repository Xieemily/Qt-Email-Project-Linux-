#ifndef USER_H
#define USER_H
#include <QString>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <QDebug>

extern QString userid_global;
extern int sockfd;
extern QString read_mail;
#endif // USER_H
