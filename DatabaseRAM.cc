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

MapNewsGroup *DatabaseRAM::ListNewsGroups()
{
    return newsgroups;
}
bool DatabaseRAM::CreateNewsGroup(string name)
{
    // Return false if NewsGroup already exists.
    bool result = false;

    // Find article
    bool found = NewsGroupExists(name);

    if (found) cout << "Found newsgroup[" << name << "] already!" << endl;
    else cout << "Couldn't find newsgroup" << endl;

    // Return false if found
    if (found) return false;

    // Create if not found
    NewsGroup ng(name);
    newsgroups->insert(make_pair(++DatabaseRAM::ID, ng));

    MapNewsGroup::iterator it = newsgroups->find(DatabaseRAM::ID);

    if (it != newsgroups->end())
        cout << "NewsGroup created successfully: " << it->second.GetName() << endl;

    result = true;
    return result;
}
bool DatabaseRAM::DeleteNewsGroup(int ngID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end() && !it->second.IsDeleted())
    {
        it->second.Delete();
        deletedGroups++;

        return true;
    }
    return false;
}

bool DatabaseRAM::NewsGroupExists(int ngID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);
    return it != newsgroups->end();
}

bool DatabaseRAM::NewsGroupExists(string title)
{
    MapNewsGroup::iterator it;
    cout << "NewsGroup.size("<<newsgroups->size() <<")" << endl;
    for (it = newsgroups->begin(); it != newsgroups->end(); ++it)
    {
        cout << "\tNG["<<it->first<<"]:" << it->second.GetName() << endl;
        if (it->second.GetName().compare(title) == 0)
            return true;
    }
    return false;
}

MapArticle *DatabaseRAM::ListArticles(int ngID)
{
    MapNewsGroup::iterator ngIt = newsgroups->find(ngID);

    if (ngIt != newsgroups->end())
    {
        return ngIt->second.ListArticles();
    }

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
    }

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
    }

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
            return article;
        }
    }

    // Could not find article.
    return 0;
}

bool DatabaseRAM::ArticleExists(int ngID, int aID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end())
    {

        return it->second.ArticleExists(aID);
    }

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
    return count;
}

size_t DatabaseRAM::NonDeletedArticleCount(int ngID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end())
    {
        return it->second.NonDeletedArticleCount();
    }
    else
    {
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
