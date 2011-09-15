#include "socket.hpp"

Socket Socket::sInstance;
Socket &gSocket = Socket::instance();

Socket::Socket()
{
#ifdef MINGW32
  WSADATA wsaData;
  mValid = (WSAStartup(MAKEWORD(1, 1), &wsaData) == 0);
#else
  mValid = true;
#endif
}

Socket::~Socket()
{
#ifdef MINGW32
  WSACleanup();
#endif
}
