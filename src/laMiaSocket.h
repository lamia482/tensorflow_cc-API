#ifndef LAMIA_SOCKET_H
#define LAMIA_SOCKET_H

#define SOCKET_DROP 0

#ifndef Linux
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <string>
typedef int socklen_t;
#else
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
typedef int SOCKET;
// tydedef sockaddr_in SOCKADDR_IN;
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

void logStr(const char *str);

typedef enum
{
  laMiaSocketTypeServer,
  laMiaSocketTypeClient
}laMiaSocketType, laMiaSocketType_t;

typedef size_t laMiaSocketSize;

class laMiaSocket
{
public:
  laMiaSocket(void);
  laMiaSocket(const laMiaSocketType &type, const int &port);
  virtual ~laMiaSocket(void);
  void setRole(const laMiaSocketType &type);
  void setPort(const int &port);
  void setBufSize(const laMiaSocketSize &size);
  int bindClient(const char *client_ip = NULL);
  void listenClient(void);
  void connectServer(const char *server_ip = NULL);
  void sendMessage(const char *buf = NULL);
  void recvMessage(char *buf = NULL);

// private:
#ifdef Linux
  void closesocket(SOCKET skt)
  {
    close(skt);
  }
  void WSACleanup(void)
  {
    logStr("WSACleanup!~");
  }
#else
  WSADATA m_WSD;
#endif // Linux

  int m_BufSize;
  SOCKET m_Server;
  SOCKET m_Client;
  char *m_pRecvMessage;
  char *m_pSendMessage;
  sockaddr_in m_AddrServ;
  int m_Port;
  laMiaSocketType m_laMiaSocketType;
};

#endif
