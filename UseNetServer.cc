#include "connectionclosedexception.h"
#include "server.h"
#include "connection.h"
#include "MessageHandler.h"
#include "IDatabase.h"
#include "DatabaseRAM.h"
#include "DatabaseDB.h"
#include "protocol.h"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;
using namespace client_server;
using namespace usenet;
using protocol::Protocol;

typedef map<int, Article> MapArticle;
typedef map<int, NewsGroup> MapNewsGroup;

void HandleListNewsGroups(MessageHandler &mh, IDatabase *db)
{
    int cmdEnd = mh.recvCode();
    if (cmdEnd != Protocol::COM_END)
    {
        cerr << "Malformed command: " << cmdEnd << ", expected: " << Protocol::COM_END << endl;
    }

    mh.sendCode(Protocol::ANS_LIST_NG);

    MapNewsGroup *ng = db->ListNewsGroups();

    int nds = db->NonDeletedNewsGroupCount();
    mh.sendIntParameter(nds);

    if (ng->size() > 0)
    {
        MapNewsGroup::iterator it;

        for (it = ng->begin(); it != ng->end(); ++it)
        {
            if (!it->second.IsDeleted())
            {
                mh.sendIntParameter(it->first);
                mh.sendStringParameter(it->second.GetName());
            }
        }
    }

    mh.sendCode(Protocol::ANS_END);
}

void HandleCreateNewsGroup(MessageHandler &mh, IDatabase *db)
{
    string newsGroupName = mh.recvStringParameter();

    int cmdEnd = mh.recvCode();
    if (cmdEnd != Protocol::COM_END)
    {
        cerr << "Malformed command: " << cmdEnd << ", expected: " << Protocol::COM_END << endl;
    }

    mh.sendCode(Protocol::ANS_CREATE_NG);
    bool newsGroupCreated = db->CreateNewsGroup(newsGroupName);

    if (newsGroupCreated)
    {
        mh.sendCode(Protocol::ANS_ACK);
    }
    else
    {
        mh.sendCode(Protocol::ANS_NAK);
        mh.sendCode(Protocol::ERR_NG_ALREADY_EXISTS);
    }
    mh.sendCode(Protocol::ANS_END);
}

void HandleDeleteNewsGroup(MessageHandler &mh, IDatabase *db)
{
    int ngID = mh.recvIntParameter();

    int cmdEnd = mh.recvCode();
    if (cmdEnd != Protocol::COM_END)
    {
        cerr << "Malformed command: " << cmdEnd << ", expected: " << Protocol::COM_END << endl;
    }

    mh.sendCode(Protocol::ANS_DELETE_NG);

    if (db->NewsGroupExists(ngID))
    {
        db->DeleteNewsGroup(ngID);
        mh.sendCode(Protocol::ANS_ACK);
    }
    else
    {
        mh.sendCode(Protocol::ANS_NAK);
        mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
    }

    mh.sendCode(Protocol::ANS_END);
}

void HandleListArticles(MessageHandler &mh, IDatabase *db)
{
    int ngID = mh.recvIntParameter();
    int cmdEnd = mh.recvCode();
    if (cmdEnd != Protocol::COM_END)
    {
        cerr << "Malformed command: " << cmdEnd << ", expected: " << Protocol::COM_END << endl;
    }

    mh.sendCode(Protocol::ANS_LIST_ART);

    if (db->NewsGroupExists(ngID))
    {
        MapArticle *result = db->ListArticles(ngID);

        mh.sendCode(Protocol::ANS_ACK);
        mh.sendIntParameter(db->NonDeletedArticleCount(ngID));

        MapArticle::iterator it;
        for (it = result->begin(); it != result->end(); ++it)
        {
            if (!it->second.IsDeleted())
            {
                mh.sendIntParameter(it->first + 1);
                mh.sendStringParameter(it->second.GetTitle());
            }
        }
    }
    else
    {
        mh.sendCode(Protocol::ANS_NAK);
        mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
    }

    mh.sendCode(Protocol::ANS_END);
}

void HandleCreateArticle(MessageHandler &mh, IDatabase *db)
{
    int ngID = mh.recvIntParameter();
    string title = mh.recvStringParameter();
    string author = mh.recvStringParameter();
    string text = mh.recvStringParameter();

    int cmdEnd = mh.recvCode();
    if (cmdEnd != Protocol::COM_END)
    {
        cerr << "Malformed command: " << cmdEnd << ", expected: " << Protocol::COM_END << endl;
    }

    mh.sendCode(Protocol::ANS_CREATE_ART);

    if (db->CreateArticle(ngID, title, author, text))
    {
        mh.sendCode(Protocol::ANS_ACK);
    }
    else
    {
        mh.sendCode(Protocol::ANS_NAK);
        mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
    }

    mh.sendCode(Protocol::ANS_END);
}

void HandleDeleteArticle(MessageHandler &mh, IDatabase *db)
{
    int ngID = mh.recvIntParameter();
    int aID = mh.recvIntParameter() - 1;

    int cmdEnd = mh.recvCode();
    if (cmdEnd != Protocol::COM_END)
    {
        cerr << "Malformed command: " << cmdEnd << ", expected: " << Protocol::COM_END << endl;
    }

    mh.sendCode(Protocol::ANS_DELETE_ART);
    if (db->NewsGroupExists(ngID))
    {
        if (db->ArticleExists(ngID, aID))
        {
            db->DeleteArticle(ngID, aID);
            mh.sendCode(Protocol::ANS_ACK);
        }
        else
        {
            mh.sendCode(Protocol::ANS_NAK);
            mh.sendCode(Protocol::ERR_ART_DOES_NOT_EXIST);
        }
    }
    else
    {
        mh.sendCode(Protocol::ANS_NAK);
        mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
    }

    mh.sendCode(Protocol::ANS_END);
}

void HandleGetArticle(MessageHandler &mh, IDatabase *db)
{
    int ngID = mh.recvIntParameter();
    int aID = mh.recvIntParameter() - 1;

    int cmdEnd = mh.recvCode();
    if (cmdEnd != Protocol::COM_END)
    {
        cerr << "Malformed command: " << cmdEnd << ", expected: " << Protocol::COM_END << endl;
    }

    mh.sendCode(Protocol::ANS_GET_ART);

    Article const *const article = db->GetArticle(ngID, aID);

    if (article != 0)
    {
        mh.sendCode(Protocol::ANS_ACK);
        mh.sendStringParameter(article->GetTitle());
        mh.sendStringParameter(article->GetAuthor());
        mh.sendStringParameter(article->GetText());
    }
    else
    {
        mh.sendCode(Protocol::ANS_NAK);

        if (!db->NewsGroupExists(ngID))
        {
            mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
        }
        else if (!db->ArticleExists(ngID, aID))
        {
            mh.sendCode(Protocol::ERR_ART_DOES_NOT_EXIST);
        }
    }

    mh.sendCode(Protocol::ANS_END);
}

int DatabaseRAM::ID = 1;
int NewsGroup::articleID = 1;

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

        if (dbOption.compare("-m") == 0 )
        {
            db = new DatabaseRAM();
            cout << "RAM database created" << endl;
        }
        else if (dbOption.compare("-db") == 0)
        {
            db = new DatabaseDB();
            cout << "DB database created" << endl;
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
        cout << "Default RAM database created" << endl;
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
                        cout << "########### HandleListNewsGroups ##########" << endl;
                        HandleListNewsGroups(mh, db);
                    }
                    break;
                    case Protocol::COM_CREATE_NG:
                    {
                        cout << "########### HandleCreateNewsGroup ##########" << endl;
                        HandleCreateNewsGroup(mh, db);
                    }
                    break;
                    case Protocol::COM_DELETE_NG:
                    {
                        cout << "########### HandleDeleteNewsGroup ##########" << endl;
                        HandleDeleteNewsGroup(mh, db);
                    }
                    break;
                    case Protocol::COM_LIST_ART:
                    {
                        cout << "########### HandleListArticles ##########" << endl;
                        HandleListArticles(mh, db);
                    }
                    break;
                    case Protocol::COM_CREATE_ART:
                    {
                        cout << "########### HandleCreateArticle ##########" << endl;
                        HandleCreateArticle(mh, db);
                    }
                    break;
                    case Protocol::COM_DELETE_ART:
                    {
                        cout << "########### HandleDeleteArticle ##########" << endl;
                        HandleDeleteArticle(mh, db);
                    }
                    break;
                    case Protocol::COM_GET_ART:
                    {
                        cout << "########### HandleGetArticle ###########" << endl;
                        HandleGetArticle(mh, db);
                    }
                    break;
                    default:
                    {
                        cerr << "Unexpected command" << endl;
                        exit(1);
                    }
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
