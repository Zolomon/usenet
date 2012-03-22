#include "connectionclosedexception.h"
#include "server.h"
#include "connection.h"
#include "MessageHandler.h"
#include "protocol.h"
#include <iostream>
#include <string>

using namespace std;
using namespace client_server;
using namespace usenet;
using protocol::Protocol;

void ListNewsGroups(const MessageHandler &mh)
{
    int code = mh.recvCode();
    cout << "recvCode: " << code << endl;
    if (code != Protocol::COM_END)
        cerr << "Malformed command...";

    cout << "Listing all NewsGroups..." << endl;
    mh.sendCode(Protocol::ANS_LIST_NG);
    mh.sendIntParameter(1);
    mh.sendIntParameter(25);
    mh.sendStringParameter("string param");
    mh.sendCode(Protocol::ANS_END);
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: UseNetServer[ MEM | DB ] port" << endl;
        exit(1);
    }

    Server server(atoi(argv[1]));
    if (!server.isReady())
    {
        cerr << "Server initialization error" << endl;
        exit(1);
    }

    while (true)
    {
        Connection *conn = server.waitForActivity();
        if (conn != 0)
        {
            try
            {
                MessageHandler mh(conn);

                int code = mh.recvCode();
                cout << "Code: " << code << endl;
                switch (code)
                {
                case Protocol::COM_LIST_NG:
                    cout << "Inside switch" << endl;
                         ListNewsGroups(mh);
                    break;
                case Protocol::COM_CREATE_NG:
                    break;
                case Protocol::COM_DELETE_NG:
                    break;
                case Protocol::COM_LIST_ART:
                    break;
                case Protocol::COM_CREATE_ART:
                    break;
                case Protocol::COM_DELETE_ART:
                    break;
                case Protocol::COM_GET_ART:
                    break;
                default:
                    break;
                }
            }
            catch (ConnectionClosedException &)
            {
                server.deregisterConnection(conn);
                delete conn;
                cout << "Client closed connection" << endl;
            }
        }
        else
        {
            server.registerConnection(new Connection);
            cout << "New client connects" << endl;
        }
    }
}
