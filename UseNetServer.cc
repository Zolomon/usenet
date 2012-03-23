#include "connectionclosedexception.h"
#include "server.h"
#include "connection.h"
#include "MessageHandler.h"
#include "IDatabase.h"
#include "DatabaseRAM.h"
#include "DatabaseDB.h"
#include "protocol.h"

#include <iostream>
#include <string>

using namespace std;
using namespace client_server;
using namespace usenet;
using protocol::Protocol;

int main(int argc, const char *argv[])
{
    if (argc > 3)
    {
        cerr << "Usage: UseNetServer port [-m|-db]" << endl;
        cerr << "-m  = RAM storage [default]" << endl;
        cerr << "-db = SQLite3 storage" << endl;
        exit(1);
    }

    IDatabase *db = 0;
    if (argc == 3)
    {
        string dbOption = argv[2];
        if (dbOption == "-m" )
        {
            db = new DatabaseRAM();
        }
        else if (dbOption == "-db")
        {
            db = new DatabaseDB();
        }
        else
        {
            cerr << "Usage: UseNetServer port [-m|-db]" << endl;
            cerr << "-m  = RAM storage [default]" << endl;
            cerr << "-db = SQLite3 storage" << endl;
            exit(1);
        }
    }
    else
    {
        // Default option
        db = new DatabaseRAM();
    }

    if (db == 0) {
        cerr << "Something went wrong, database not connected." << endl;
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
                    db->ListNewsGroups(mh);
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
