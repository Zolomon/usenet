#include "connectionclosedexception.h"
#include "server.h"
#include "connection.h"
#include "MessageHandler.h"
#include "IDatabase.h"
#include "DatabaseRAM.h"
//#include "DatabaseDB.h"
#include "protocol.h"

#include <iostream>
#include <string>

using namespace std;
using namespace client_server;
using namespace usenet;
using protocol::Protocol;

int main(int argc, const char *argv[])
{
    if (argc > 3 || argc == 1)
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
            db->CreateNewsGroup("java.google.com");
            db->CreateNewsGroup("python.google.com");
            db->CreateNewsGroup("c++.google.com");
            db->CreateNewsGroup("c.google.com");
            db->CreateNewsGroup("haskell.google.com");
            db->CreateNewsGroup("ruby.google.com");
            db->CreateNewsGroup("scheme.google.com");
        }
        else if (dbOption == "-db")
        {
            //db = new DatabaseDB();
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
                while (conn->isConnected())
                {
                    int code = mh.recvCode();
                    switch (code)
                    {
                    case Protocol::COM_LIST_NG:
                    {
                        int cmdEnd = mh.recvCode();
                        cout << "recvCode: " << cmdEnd << endl;
                        if (cmdEnd != Protocol::COM_END)
                            cerr << "Malformed command...";

                        cout << "Listing all NewsGroups..." << endl;
                        mh.sendCode(Protocol::ANS_LIST_NG);

                        vector<NewsGroup> ng = db->ListNewsGroups();
                        mh.sendIntParameter(ng.size());
                        for (std::vector<NewsGroup>::size_type i = 0; i != ng.size(); ++i)
                        {
                            mh.sendIntParameter(i);
                            mh.sendStringParameter(ng[i].GetName());
                        }

                        mh.sendCode(Protocol::ANS_END);
                    }
                    break;
                    case Protocol::COM_CREATE_NG:
                    {
                        string newsGroupName = mh.recvStringParameter();

                        int cmdEnd = mh.recvCode();
                        cout << "recvCode: " << cmdEnd << endl;
                        if (cmdEnd != Protocol::COM_END)
                            cerr << "Malformed command...";

                        mh.sendCode(Protocol::ANS_CREATE_NG);

                        if (db->CreateNewsGroup(newsGroupName)) {
                            mh.sendCode(Protocol::ANS_ACK);
                        } else {
                            mh.sendCode(Protocol::ANS_NAK);
                            mh.sendCode(Protocol::ERR_NG_ALREADY_EXISTS);
                        }
                        mh.sendCode(Protocol::ANS_END);
                    }
                    break;
                    case Protocol::COM_DELETE_NG:
                        //db->DeleteNewsGroup(mh);
                        break;
                    case Protocol::COM_LIST_ART:
                        //db->ListArticles(mh);
                        break;
                    case Protocol::COM_CREATE_ART:
                        //db->CreateArticle(mh);
                        break;
                    case Protocol::COM_DELETE_ART:
                        //db->DeleteArticle(mh);
                        break;
                    case Protocol::COM_GET_ART:
                        //db->GetArticle(mh);
                        break;
                    default:
                        cerr << "Unexpected command" << endl;
                        exit(1);
                        break;
                    }
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
