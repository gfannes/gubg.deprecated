#include <iostream>
#include <sstream>

#include "comStation.hpp"
#ifndef MINGW32
#include <arpa/inet.h>
#endif
#include "message.hpp"

using namespace std;

ComStation::~ComStation()
{
  finish();
  if (mAcceptThread)
    delete mAcceptThread;
}

bool ComStation::start()
{
  int yes=1;
  int ret;

  if ((mListenSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      cerr << "Could not create the listen socket." << endl;
      return false;
    }

  if (setsockopt(mListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(int)) == -1)
    {
      cerr << "Could not set the reuse option of the socket." << endl;
      return false;
    }

  mAddress.sin_family = AF_INET;         // host byte order
  mAddress.sin_port = htons(mHere.port());     // short, network byte order
  mAddress.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
  memset(mAddress.sin_zero, '\0', sizeof(mAddress.sin_zero));

  if (bind(mListenSocket, (struct sockaddr *)&mAddress, sizeof(mAddress)) == -1)
    {
      cerr << "Could not bind." << endl;
      return false;
    }

  if (listen(mListenSocket, mNrPending) == -1)
    {
      cerr << "Could not listen." << endl;
      return false;
    }

  // Start the accept thread
  mContinue2Listen=true;
  mAcceptThread = new AcceptThread(mListenSocket, &mContinue2Listen, &mStagingM);
  mAcceptThread->start();

  return true;
}

bool ComStation::finish()
{
  cout << "Closing ComStation " << this << endl;
  mContinue2Listen=false;
  if (mAcceptThread && mAcceptThread->isRunning())
    {
      mAcceptThread->finish();
    }
  return true;
}

class Add2MessageList
  {
  public:
    Add2MessageList(Message *message):
        mMessage(message) {};
    bool execute4Mutex(MessageListT *messageList)
    {
      cout << "Message list contains " << messageList->size() << " elements" << endl;
      messageList->push_back(mMessage);
      return true;
    }
  private:
    Message *mMessage;
  };

bool AcceptThread::execute()
{
  struct sockaddr_in peer_addr;
#ifdef MINGW32
  typedef int socklen_t;
#endif
  socklen_t sin_size;
  int new_fd;

  struct timeval tv;
  fd_set readfds;

  string buffer;

  while (*mContinue2Listen)
    {
//        cout << "Waiting for a new connect to accept it..." << endl;

      tv.tv_sec = 1;
      tv.tv_usec = 0;
      FD_ZERO(&readfds);
      FD_SET(mListenSocket, &readfds);
      // don't care about writefds and exceptfds:
      select(mListenSocket+1, &readfds, NULL, NULL, &tv);
      if (FD_ISSET(mListenSocket, &readfds))
        {
          cout << "Incoming connection detected." << endl;

          sin_size = sizeof(peer_addr);
          if ((new_fd = accept(mListenSocket, (struct sockaddr *)&peer_addr, &sin_size)) == -1)
            {
              cerr << "Could not accept connection." << endl;
              continue;
            }
          cout << "Received connection from " << inet_ntoa(peer_addr.sin_addr) << endl;

          if (!ComStation::read(new_fd, buffer))
            {
              cerr << "Could not receive the buffer." << endl;
              continue;
            }
          cout << "I received " << buffer.size() << " bytes." << endl;

          istringstream iss(buffer);
          void *pContent;
          int typeID;
          Serialize::unserializeWithType(pContent,typeID,iss);
          cout << "I received an object of type " << typeID << endl;
          Message *message=new Message(eMessageEvent, typeID, pContent);
          Add2MessageList add2Staging(message);
          mStagingM->access(add2Staging);

#ifdef MINGW32
          closesocket(new_fd);
#else
          close(new_fd);
#endif
        }
      else
        {
//            cout << "Time out." << endl;
        }
    }
  return false;
}

bool ComStation::write(ComPoint compoint, string &buffer)
{
  int sockfd;
  struct hostent *he;
  struct sockaddr_in peer_addr;
  bool res;

  if ((he=gethostbyname(compoint.hostname().c_str())) == NULL)
    {
      cerr << "Could not het host by name." << endl;
      return false;
    }

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
      cerr << "Could not create the socket." << endl;
      return false;
    }

  peer_addr.sin_family = AF_INET;    // host byte order
  peer_addr.sin_port = htons(compoint.port());  // short, network byte order
  peer_addr.sin_addr = *((struct in_addr *)he->h_addr);
  memset(peer_addr.sin_zero, 0x00, sizeof(peer_addr.sin_zero));

  if (connect(sockfd, (struct sockaddr *)&peer_addr, sizeof peer_addr) == -1)
    {
      cerr << "Could not connect to peer." << endl;
      return false;
    }

  res=write(sockfd,buffer);

#ifdef MINGW32
  closesocket(sockfd);
#else
  close(sockfd);
#endif

  return res;
}


bool ComStation::writeBuffer(int fd, char const *buffer, int bytes2Send)
{
  int alreadySent = 0;
  int justSent;

  while (alreadySent < bytes2Send)
    {
      justSent = ::send(fd, buffer+alreadySent, bytes2Send, 0);
      if (justSent == -1)
        break;
      alreadySent += justSent;
      bytes2Send -= justSent;
    }
  return (justSent == -1 ? false : true);
}

bool ComStation::write(int fd, string &buffer)
{
  int length=buffer.size();
  return writeBuffer(fd, reinterpret_cast<char*>(&length), sizeof(length)) && writeBuffer(fd, buffer.data(), length);
}

bool ComStation::readBuffer(int fd, char *buffer, int bytes2Receive)
{
  int alreadyReceived = 0;
  int justReceived;

  while (alreadyReceived < bytes2Receive)
    {
      justReceived = recv(fd, buffer+alreadyReceived, bytes2Receive, 0);
      if (justReceived == -1)
        break;
      alreadyReceived += justReceived;
      bytes2Receive -= justReceived;
    }
  return (justReceived == -1 ? false : true);
}

bool ComStation::read(int fd, string &buffer)
{
  int length;
  if (!readBuffer(fd, reinterpret_cast<char*>(&length), sizeof(length)))
    {
      cerr << "Failed to receive the length of the buffer." << endl;
      return false;
    }
  buffer.resize(length);
  return readBuffer(fd, const_cast<char*>(buffer.data()), length);
}

class MoveStaging2Messages
  {
  public:
    MoveStaging2Messages(Mutex<MessageListT> *messageListM):
        mMessageListM(messageListM) {};
    bool execute4Mutex(MessageListT *staging)
    {
      Message *message;
      while (!staging->empty())
        {
          message = staging->front();
          staging->pop_front();
          Add2MessageList add2MessageList(message);
          mMessageListM->access(add2MessageList);
        }
      return true;
    };
  private:
    Mutex<MessageListT> *mMessageListM;
  };

bool ComStation::moveStaging2Messages()
{
  MoveStaging2Messages move(&mMessagesM);
  return mStagingM.access(move);
}
