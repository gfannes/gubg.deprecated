#include <iostream>
#include <string>

#include "thread.hpp"
#include "sleep.hpp"

using namespace std;

class Printer: public Thread
  {
  public:
    Printer(string str):
        _toPrint(str) {};
  protected:
    virtual bool execute()
    {
      int i;
      for (i=0;i<3;i++)
        {
          nanosleep(0, 1000);
          cout << i << " " << _toPrint << endl;
        }
    }
  public:
    string _toPrint;
  };

class Grabber: public Thread
  {
  public:
    Grabber(string id,Mutex<int> *token):
        _id(id),
        _token(token) {};
    bool execute4Mutex(int *token)
    {
      (*token)++;
      cout << "Too late, the token is taken by " << _id << " and has value " << *token << endl;
      sleep(1);
    }
  protected:
    virtual bool execute()
    {
      int i;
      for (i=0;i<3;i++)
        {
          nanosleep(0, 1000);
          _token->access(*this);
        }
    }
  private:
    string _id;
    Mutex<int> *_token;
  };

int main()
{
  Printer p1("aaa");
  Printer p2("bbb");
  Printer p3("ccc");
  Printer p4("ddd");
  Printer p5("eee");
  p1.start();
  p2.start();
  p3.start();
  p4.start();
  p5.start();
  p1.finish();
  p2.finish();
  p3.finish();
  p4.finish();
  p5.finish();

  int i=0;
  Mutex<int> token(&i);
  Grabber g1("aaa",&token);
  Grabber g2("bbb",&token);
  Grabber g3("ccc",&token);
  g1.start();
  g2.start();
  g3.start();
  g1.finish();
  g2.finish();
  g3.finish();

  return 0;
}
