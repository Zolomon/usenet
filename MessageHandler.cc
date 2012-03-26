#include "MessageHandler.h"
#include "protocol.h"
#include <iostream>
#include <string>
#include <sstream>

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

int MessageHandler::recvInt()
const throw(ConnectionClosedException)
{
    int b1 = recvByte();
    int b2 = recvByte();
    int b3 = recvByte();
    int b4 = recvByte();

    return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

int MessageHandler::recvIntParameter()
const throw(ConnectionClosedException)
{
    int code = recvCode();
    if (code != Protocol::PAR_NUM)
    {
        throw ConnectionClosedException();
    }
    return recvInt();
}

string MessageHandler::recvStringParameter()
const throw(ConnectionClosedException)
{
    int code = recvCode();
    if (code != Protocol::PAR_STRING)
    {
        throw ConnectionClosedException();
    }
    int n = recvInt();
    if (n < 0)
    {
        throw ConnectionClosedException();
    }
    stringstream ss;
    for (int i = 0; i != n; ++i)
    {
        unsigned char readChar = connection->read();
        ss << readChar;
    }
    string result = ss.str();
    return result;
}

}