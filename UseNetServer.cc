#include "connectionclosedexception.h"
#include "server.h"
#include "connection.h"
#include "MessageHandler.h"
#include "IDatabase.h"
#include "DatabaseRAM.h"
//#include "DatabaseDB.h"
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

    cout << "Listing all NewsGroups..." << endl;
    mh.sendCode(Protocol::ANS_LIST_NG);

    MapNewsGroup* ng = db->ListNewsGroups();
    if (ng == 0) cout << "Could not fetch newsgroup..." << endl;
    cout << "NG size: " << ng->size() << endl;
    cout << ng->size() << " " << db->NonDeletedNewsGroupCount() << endl;
    mh.sendIntParameter(db->NonDeletedNewsGroupCount());

    MapNewsGroup::iterator it;

    for (it = ng->begin(); it != ng->end(); ++it) {
        if (!it->second.IsDeleted()) {
            mh.sendIntParameter(it->first);
            mh.sendStringParameter(it->second.GetName());
        }
    }

    mh.sendCode(Protocol::ANS_END);

    //delete ng;
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

    if (db->CreateNewsGroup(newsGroupName))
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
    cout << "Listing Articles for NewsGroup[" << ngID << "]" << endl;

    int cmdEnd = mh.recvCode();
    if (cmdEnd != Protocol::COM_END)
    {
        cerr << "Malformed command: " << cmdEnd << ", expected: " << Protocol::COM_END << endl;
    }

    mh.sendCode(Protocol::ANS_LIST_ART);

    if (db->NewsGroupExists(ngID))
    {
        MapArticle* result = db->ListArticles(ngID);
        
        mh.sendCode(Protocol::ANS_ACK);
        mh.sendIntParameter(db->NonDeletedArticleCount(ngID));

	cout << "Article["<<ngID<<"].Count: " << db->NonDeletedArticleCount(ngID) << endl;
        MapArticle::iterator it;

        for (it = result->begin(); it != result->end(); ++it) {
            if (!it->second.IsDeleted()) {
                mh.sendIntParameter(it->first + 1);
                mh.sendStringParameter(it->second.GetTitle());
            }
        }

	//delete result;
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
    cout << "Deleting NewsGroup["<<ngID<<"].Article["<<aID<<"]"<<endl;

    int cmdEnd = mh.recvCode();
    if (cmdEnd != Protocol::COM_END)
    {
        cerr << "Malformed command: " << cmdEnd << ", expected: " << Protocol::COM_END << endl;
    }

    mh.sendCode(Protocol::ANS_DELETE_ART);
    if (db->NewsGroupExists(ngID))
    {
        cout << "\tNewsGroup exists ..." << endl;
        cout << "\tArticle Count: " << db->NonDeletedArticleCount(ngID) << endl;

        //cout << "Article["<<ngID<<"]["<<aID<<"]: " << db->GetArticle(ngID, aID)->GetTitle() << endl;

        cout << "Prior deletion: " << endl;
        cout << db->ToString() << endl;

        if (db->ArticleExists(ngID, aID))
        {
            cout << "\t\tArticle exists ..." << endl;
            db->DeleteArticle(ngID, aID);
            mh.sendCode(Protocol::ANS_ACK);
        }
        else
        {
            cout << ">>> \t\tArticle DOES NOT EXIST ..." << endl;
            mh.sendCode(Protocol::ANS_NAK);
            mh.sendCode(Protocol::ERR_ART_DOES_NOT_EXIST);
        }
    }
    else
    {
        cout << ">>> \tNewsGroup DOES NOT EXIST ..." << endl;
        mh.sendCode(Protocol::ANS_NAK);
        mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
    }

    cout << "After deletion: " << endl;
    cout << db->ToString() << endl;

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
        else
        {
            cerr << "Could not delete article." << endl;
            exit(1);
        }
    }

    mh.sendCode(Protocol::ANS_END);
}

  int DatabaseRAM::ID = 0;
  int NewsGroup::articleID = 0;

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
            // db->CreateNewsGroup("java.google.com");
            // db->CreateNewsGroup("python.google.com");
            // db->CreateNewsGroup("c++.google.com");
            // db->CreateNewsGroup("c.google.com");
            // db->CreateNewsGroup("haskell.google.com");
            // db->CreateNewsGroup("ruby.google.com");
            // db->CreateNewsGroup("scheme.google.com");
            // db->CreateArticle(0, "Java1", "Zolomon", "Java 101");
            // db->CreateArticle(0, "Java1", "Zolomon", "Java 101.1");
            // db->CreateArticle(0, "Java2", "Zolomon", "Java 102");
            // db->CreateArticle(0, "Java3", "Zolomon", "Java 103");
            // db->CreateArticle(0, "Java4", "Zolomon", "Java 104");
            // db->CreateArticle(1, "Python1", "Zolomon", "Python 101");
            // db->CreateArticle(1, "Python2", "Zolomon", "Python 102");
            // db->CreateArticle(1, "Python3", "Zolomon", "Python 103");
            // db->CreateArticle(1, "Python4", "Zolomon", "Python 104");
            // db->CreateArticle(1, "Python5", "Zolomon", "Python 105");
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
                        cout << "#####################" << endl;
                        HandleListNewsGroups(mh, db);
                    }
                    break;
                    case Protocol::COM_CREATE_NG:
                    {
                        cout << "#####################" << endl;
                        HandleCreateNewsGroup(mh, db);
                    }
                    break;
                    case Protocol::COM_DELETE_NG:
                    {
                        cout << "#####################" << endl;
                        HandleDeleteNewsGroup(mh, db);
                    }
                    break;
                    case Protocol::COM_LIST_ART:
                    {
                        cout << "#####################" << endl;
                        HandleListArticles(mh, db);
                    }
                    break;
                    case Protocol::COM_CREATE_ART:
                    {
                        cout << "#####################" << endl;
                        HandleCreateArticle(mh, db);
                    }
                    break;
                    case Protocol::COM_DELETE_ART:
                    {
                        cout << "#####################" << endl;
                        HandleDeleteArticle(mh, db);
                    }
                    break;
                    case Protocol::COM_GET_ART:
                    {
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
