#include "MessageHandler.h"
#include "protocol.h"
#include <iostream>

using namespace protocol;
using namespace std;

namespace usenet
{
void MessageHandler::sendByte(int code)
const throw(ConnectionClosedException)
{
    connection->write(code);
}

void MessageHandler::sendCode(int code)
const throw(ConnectionClosedException)
{
    sendByte(code);
}

void MessageHandler::sendInt(int value)
const throw(ConnectionClosedException)
{
    sendByte((value >> 24) & 0xFF);
    sendByte((value >> 16) & 0xFF);
    sendByte((value >> 8) & 0xFF);
    sendByte(value & 0xFF);
}

void MessageHandler::sendIntParameter(int param)
const throw(ConnectionClosedException)
{
    sendCode(Protocol::PAR_NUM);
    sendInt(param);
}

void MessageHandler::sendStringParameter(string param)
const throw(ConnectionClosedException)
{
    sendCode(Protocol::PAR_STRING);
    sendInt(param.size());
    for (string::size_type i = 0; i != param.size(); ++i)
    {
        sendByte(param[i]);
    }
}

int MessageHandler::recvByte()
const throw(ConnectionClosedException)
{
    int code;
    code = connection->read();
    return code;
}

int MessageHandler::recvCode()
const throw(ConnectionClosedException)
{
    int code = recvByte();
    return code;
}

void MessageHandler::error(const char *msg) const
{
    cerr << "MessageHandler: " << msg << endl;
}
}