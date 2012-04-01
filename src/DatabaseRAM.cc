#include "DatabaseRAM.h"
#include "MessageHandler.h"
#include <iostream>
#include <algorithm>
#include "protocol.h"
#include <utility>
#include <sstream>

using namespace std;
using protocol::Protocol;

namespace usenet
{
typedef map<int, NewsGroup> MapNewsGroup;
typedef map<int, Article> MapArticle;

DatabaseRAM::~DatabaseRAM()
{
    //delete newsgroups;
}

MapNewsGroup *DatabaseRAM::ListNewsGroups()
{
    MapNewsGroup *ng = new MapNewsGroup(newsgroups->begin(), newsgroups->end());
    return ng;
    //return newsgroups;
}

bool DatabaseRAM::CreateNewsGroup(string name)
{
    // Return false if NewsGroup already exists.
    bool result = false;

    // Find article
    bool found = NewsGroupExists(name);

    // Return false if found
    if (found) return false;

    // Create if not found
    NewsGroup ng(name);
    newsgroups->insert(make_pair(++DatabaseRAM::ID, ng));

    result = true;
    cout << "\tCreated list of NewsGroups successfully ..." << endl;
    return result;
}
bool DatabaseRAM::DeleteNewsGroup(int ngID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end() && !it->second.IsDeleted())
    {
        it->second.Delete();
        deletedGroups++;
        cout << "\tDeleted NewsGroup successfully ..." << endl;
        return true;
    }
    cerr << "\tCould not delete NewsGroup ..." << endl;
    return false;
}

bool DatabaseRAM::NewsGroupExists(int ngID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);
    if (it != newsgroups->end()) cout << "\tNewsGroup exists ..." << endl;
    else cerr << "\tNewsGroup could not be found ..." << endl;
    return it != newsgroups->end();
}

bool DatabaseRAM::NewsGroupExists(string title)
{
    MapNewsGroup::iterator it;
    for (it = newsgroups->begin(); it != newsgroups->end(); ++it)
    {
        if (it->second.GetName().compare(title) == 0)
        {
            cout << "\tNewsGroup exists ..." << endl;
            return true;
        }
    }
    cerr << "\tNewsGroup could not be found ..." << endl;
    return false;
}

MapArticle *DatabaseRAM::ListArticles(int ngID)
{
    MapNewsGroup::iterator ngIt = newsgroups->find(ngID);

    if (ngIt != newsgroups->end())
    {
        cout << "\tCreated list of Articles successfully ..." << endl;
        return ngIt->second.ListArticles();
    }

    cerr << "\tNewsGroup could not be found ..." << endl;
    
    // If not found, return NULL-pointer...
    return 0;
}
bool DatabaseRAM::CreateArticle(int ngID, string title, string author, string text)
{
    // Return false if article already exists.
    bool result = false;

    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end())
    {
        // Find article
        bool found = it->second.FindArticle(title);

        // Return false if found
        if (found) return false;

        // Create if not found
        it->second.CreateArticle(title, author, text);
        result = true;
        cout << "\tArticle created successfully ..." << endl;
    }
    cerr << "\tCould not create Article ..." << endl;
    return result;
}
bool DatabaseRAM::DeleteArticle(int ngID, int aID)
{
    bool result = false;
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end())
    {
        it->second.DeleteArticle(aID);
        result = true;
        cout << "\tArticle deleted successfully ..." << endl;
    }
    cerr << "\tCould not delete Article ..." << endl;
    return result;
}

Article const *DatabaseRAM::GetArticle(int ngID, int aID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end())
    {
        if (it->second.ArticleExists(aID))
        {
            Article const *article = it->second.GetArticle(aID);
            cout << "\tArticle retrieved successfully ..." << endl;
            return article;
        }
    }

    cerr << "\tCould not retrieve Article ..." << endl;

    // Could not find article.
    return 0;
}

bool DatabaseRAM::ArticleExists(int ngID, int aID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end())
    {
        cout << "\tArticle exists ..." << endl;
        return it->second.ArticleExists(aID);
    }
    cerr << "\tArticle could not be found ..." << endl;
    return false;
}

size_t DatabaseRAM::NonDeletedNewsGroupCount()
{
    MapNewsGroup::iterator it;

    size_t count = 0;
    for ( it = newsgroups->begin(); it != newsgroups->end(); ++it)
    {

        if (!it->second.IsDeleted())
        {
            ++count;
        }
    }
    cout << "\tNondeleted NewsGroup Count: " << count << " ..." << endl;
    return count;
}

size_t DatabaseRAM::NonDeletedArticleCount(int ngID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end())
    {
        cout << "\tNondeleted Article Count: " << it->second.NonDeletedArticleCount() << " ..." << endl;
        return it->second.NonDeletedArticleCount();
    }
    else
    {
        cout << "\tNondeleted Article Count: 0 ..." << endl;
        return 0;
    }
}

string DatabaseRAM::ToString()
{
    stringstream ss;

    MapNewsGroup::iterator it;
    for (it = newsgroups->begin(); it != newsgroups->end(); ++it)
    {
        ss << it->first << ":" << it->second.ToString() << endl;
    }
    return ss.str();
}
}
