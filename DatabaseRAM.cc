#include "DatabaseRAM.h"
#include "MessageHandler.h"
#include <iostream>
#include <algorithm>
#include "protocol.h"

using std::cout;
using std::endl;
using std::cerr;
using protocol::Protocol;

namespace usenet
{
vector<NewsGroup> DatabaseRAM::ListNewsGroups()
{
    vector<NewsGroup> ng;

    for (std::vector<NewsGroup>::size_type i = 0;
            i < newsgroups.size(); ++i)
    {
        if (!newsgroups[i].isDeleted)
            ng.push_back(newsgroups[i]);
    }

    return ng;
}
bool DatabaseRAM::CreateNewsGroup(string name)
{
    // Return false if NewsGroup already exists.
    bool result = false;

    // Find article
    bool found = FindNewsGroup(name);

    // Return false if not found
    if (found) return false;

    // Create if not found
    NewsGroup ng(name);
    newsgroups.push_back(name);
    
    result = true;
    return result;
}
bool DatabaseRAM::DeleteNewsGroup(int ngID)
{
    bool result = false;

    if (static_cast<vector<NewsGroup>::size_type>(ngID) < newsgroups.size()
            && ngID > 0)
    {
        newsgroups[ngID].Delete();
    }

    return result;
}
vector<Article> DatabaseRAM::ListArticles(int ngID)
{
    vector<Article> articles;
    if (static_cast<vector<NewsGroup>::size_type>(ngID) < newsgroups.size()
            && ngID > 0)
        articles = newsgroups[ngID].ListArticles();

    return articles;
}
bool DatabaseRAM::CreateArticle(int ngID, string title, string author, string text)
{
    // Return false if article already exists.
    bool result = false;

    if (static_cast<vector<NewsGroup>::size_type>(ngID) < newsgroups.size()
            && ngID > 0)
    {
        // Find article
        bool found = newsgroups[ngID].FindArticle(title);

        // Return false if not found
        if (found) return false;

        // Create if not found
        newsgroups[ngID].CreateArticle(title, author, text);
        result = true;
    }

    return result;
}
bool DatabaseRAM::DeleteArticle(int ngID, int aID)
{
    bool result = false;

    if (static_cast<vector<NewsGroup>::size_type>(ngID) < newsgroups.size()
            && ngID > 0)
    {
        newsgroups[static_cast<vector<NewsGroup>::size_type>(ngID)].articles[static_cast<vector<Article>::size_type>(aID)].Delete();
        result = true;
    }

    return result;
}
Article const *const DatabaseRAM::GetArticle(int ngID, int aID)
{
    if (static_cast<vector<NewsGroup>::size_type>(ngID) < newsgroups.size() && ngID > 0)
        return newsgroups[static_cast<vector<NewsGroup>::size_type>(ngID)].GetArticle(aID);

    // Could not find article.
    return 0;
}
bool DatabaseRAM::FindNewsGroup(string name) const
{
    vector<NewsGroup>::const_iterator it = find_if(
        newsgroups.begin(),
        newsgroups.end(),
        bind2nd(FindNewsGroupByName(), name));

    return it != newsgroups.end();
}
}