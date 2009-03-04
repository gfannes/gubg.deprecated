#include <iostream>
#include <sstream>

#include "comStation.hpp"
#include "initialization.hpp"

using namespace std;

class Checker
{
public:
    bool execute4Mutex(MessageListT *messages)
        {
            cout << "I found " << messages->size() << " messages" << endl;
            return true;
        }
};

int main(int argc, char **argv)
{
    initialize();
    ComPoint cp("localhost",1234);
    cout << cp.toStr() << endl;
    if ( argc > 1)
    {
        if (strcmp(argv[1],"client") == 0)
        {
            cout << "Starting client." << endl;
            string message;
            ostringstream oss;
            int i;
            for (i=0;i<1000000;i++)
                oss << "TestMessage" << endl;
            message=oss.str();
            ComStation::send(cp,message);
            cout << "sending is af" << endl;
        } else if (strcmp(argv[1],"server") == 0)
        {
            ComStation cs(cp);
            cs.start();
            sleep(10);

            Checker checker;
            cs.checkMessages(checker);
        }
    } else
    {
        ComStation cs(cp);

        cs.start();
        sleep(1);
    }
    return 0;
}
