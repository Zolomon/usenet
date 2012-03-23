#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "connectionclosedexception.h"
#include "connection.h"
#include <string>

using std::string;
using client_server::Connection;
using client_server::ConnectionClosedException;

namespace usenet
{
class MessageHandler
{
public:
    MessageHandler(Connection *conn) : connection(conn) {};
    ~MessageHandler()
    {
        delete connection;
    }

    void sendCode(int code) const throw(ConnectionClosedException);
    void sendInt(int value) const throw(ConnectionClosedException);
    void sendIntParameter(int param) const throw(ConnectionClosedException);
    void sendStringParameter(string param) const throw(ConnectionClosedException);

    int recvCode() const throw(ConnectionClosedException);
    int recvInt() const throw(ConnectionClosedException);
    int recvIntParameter() const throw(ConnectionClosedException);
    string recvStringParameter() const throw(ConnectionClosedException);

    // TODO: Remove this shite, it's just here for Clang.
    void TOREMOVE(string msg);
private:
    void sendByte(int code) const throw(ConnectionClosedException);
    int recvByte() const throw(ConnectionClosedException);

    Connection *connection;
};
}
#endif
