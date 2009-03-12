#ifndef message_h
#define message_h

// The message to be sent to a ComPoint via a ComStation

#include <iostream>
#include <string>

#include "serialize.hpp"

using namespace std;

namespace gubg
{
// Message types
    enum MessageTypeE
    {
	eMessageRequest,
	eMessageResponse,
	eMessageEvent
    };

    class Message
    {
    public:
	Message(){};
	Message(MessageTypeE messageType, int typeID,void *pContent):
	    mMessageType(messageType),
	    mType(typeID),
	    mContent(pContent){};
	template< class T >
	Message(MessageTypeE t,T &c)
	    {
		mMessageType = t;
		mType = Serialize::typeID<T>();
		mContent = &c;
	    };
	static Message* allocate4Serialize(){return (new Message());};
	bool serialize4Serialize(ostream &output)
	    {
		cerr << "Message::toBin is not finished.";
		return false;
	    };
	bool unserialize4Serialize(istream &input){cerr << "Message::fromBin is not finished";return false;};
    private:
	MessageTypeE mMessageType;
	int mType;
	void *mContent;
    };

    namespace Serialize
    {
	SetType(Message,eTypeIDMessage);
    }
};
#endif
