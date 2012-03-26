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
    // vector<NewsGroup> ng;

    // for (std::vector<NewsGroup>::size_type i = 0;
    //         i < newsgroups.size(); ++i)
    // {
    //     if (!newsgroups[i].isDeleted)
    //         ng.push_back(newsgroups[i]);
    // }

    // return ng;
    return newsgroups;
}
bool DatabaseRAM::CreateNewsGroup(string name)
{
    // Return false if NewsGroup already exists.
    bool result = false;

    // Find article
    bool found = FindNewsGroup(name);

    // Return false if found
    if (found) return false;

    // Create if not found
    NewsGroup ng(name);
    newsgroups.push_back(name);

    result = true;
    return result;
}
void DatabaseRAM::DeleteNewsGroup(int ngID)
{
    newsgroups[ngID].Delete();
    deletedGroups++;
}

bool DatabaseRAM::NewsGroupExists(int ngID)
{
    return  static_cast<vector<NewsGroup>::size_type>(ngID) < newsgroups.size() &&
            !newsgroups[static_cast<vector<NewsGroup>::size_type>(ngID)].IsDeleted();
}

vector<Article> DatabaseRAM::ListArticles(int ngID)
{
    vector<Article> articles;
    if (NewsGroupExists(ngID))
        articles = newsgroups[static_cast<vector<Article>::size_type>(ngID)].ListArticles();

    return articles;
}
bool DatabaseRAM::CreateArticle(int ngID, string title, string author, string text)
{
    // Return false if article already exists.
    bool result = false;

    if (NewsGroupExists(ngID))
    {
        // Find article
        bool found = newsgroups[static_cast<vector<NewsGroup>::size_type>(ngID)].FindArticle(title);

        // Return false if found
        if (found) return false;

        // Create if not found
        cout << "Creating NewsGroup[" << ngID << "].Article(" << title << "," << author << "," << text << ")" << endl;
        newsgroups[ngID].CreateArticle(title, author, text);
        result = true;
    }

    return result;
}
bool DatabaseRAM::DeleteArticle(int ngID, int aID)
{
    bool result = false;

    if (NewsGroupExists(ngID))
    {
        newsgroups[static_cast<vector<NewsGroup>::size_type>(ngID)].DeleteArticle(aID-1);
        result = true;
    }

    return result;
}
Article const *const DatabaseRAM::GetArticle(int ngID, int aID)
{
    if (NewsGroupExists(ngID))
    {
        if (newsgroups[static_cast<vector<NewsGroup>::size_type>(ngID)].ArticleExists(aID))
        {
            Article const *const article = newsgroups[static_cast<vector<NewsGroup>::size_type>(ngID)].GetArticle(aID);
            cout << "NewsGroup[" << ngID << "].Article[" << aID << "]" << endl
                 << "\tTitle:\t\t" << article->GetTitle() << endl
                 << "\tAuthor:\t\t" << article->GetAuthor() << endl;
                 //<< "\tText:\t\t" << article->GetText() << endl;
            return article;
        }
        else
            cerr << "Article[" << aID << "] does not exist." << endl;
    }
    else
    {
        cerr << "NewsGroup[" << ngID << "] does not exist." << endl;
    }

    // Could not find article.
    return 0;
}

bool DatabaseRAM::ArticleExists(int ngID, int aID)
{
    return NewsGroupExists(ngID) &&
           newsgroups[static_cast<vector<NewsGroup>::size_type>(ngID)].ArticleExists(aID);
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