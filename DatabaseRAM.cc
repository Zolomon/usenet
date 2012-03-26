#include "DatabaseRAM.h"
#include "MessageHandler.h"
#include <iostream>
#include <algorithm>
#include "protocol.h"
#include <utility>

using std::cout;
using std::endl;
using std::cerr;
using protocol::Protocol;
using std::make_pair;

namespace usenet
{
typedef map<int, NewsGroup> MapNewsGroup;
typedef map<int, Article> MapArticle;

MapNewsGroup* DatabaseRAM::ListNewsGroups()
{
    return newsgroups;
}
bool DatabaseRAM::CreateNewsGroup(string name)
{

  cout << "Creating NewsGroup: " << name << endl;
    // Return false if NewsGroup already exists.
    bool result = false;

    // Find article
    bool found = FindNewsGroup(name);

    // Return false if found
    if (found) return false;

    cout << "\tNewsGroup did not exist, so we create it ..." << endl;

    // Create if not found
    NewsGroup ng(name);
    newsgroups->insert(make_pair(++DatabaseRAM::ID, ng));

    MapNewsGroup::iterator it = newsgroups->find(DatabaseRAM::ID);

    if (it != newsgroups->end()) cout << "We found it: " << it->second.GetName() << endl;

    cout << "\t\tNewsGroup created with ID: " << DatabaseRAM::ID << endl;
    cout << "Size: " << newsgroups->size() << endl;

    result = true;
    return result;
}
bool DatabaseRAM::DeleteNewsGroup(int ngID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end() && !it->second.IsDeleted())
    {
        cout << "Deleting NewsGroup[" << ngID << "]" << endl;

        it->second.Delete();
        deletedGroups++;

        if (deletedGroups >= newsgroups->size())
            cout << "newsgroups->size(" << newsgroups->size() << ") && deletedGroups = " << deletedGroups << endl;

        return true;
    }
    return false;
}

bool DatabaseRAM::NewsGroupExists(int ngID)
{
    MapNewsGroup::iterator it = newsgroups->find(ngID);
    return it != newsgroups->end();
}

MapArticle* DatabaseRAM::ListArticles(int ngID)
{
    MapNewsGroup::iterator ngIt = newsgroups->find(ngID);

    if (ngIt != newsgroups->end()) {
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
        cout << "Creating NewsGroup[" << ngID << "].Article(" << title << "," << author << "," << text << ")" << endl;
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
        cout << "Deleting NewsGroup[" << ngID << "].Article[" << aID << "]" << endl;
	cout << "Name of article to be deleted" << it->second.GetArticle(aID)->GetTitle() << endl;
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
    MapNewsGroup::iterator it = newsgroups->find(ngID);

    if (it != newsgroups->end())
    {
      cout << "Not at the end of newsgroups..." << endl;
        return it->second.ArticleExists(aID);
    }
    cout << "At the end of newsgroups :D FU Bengt." << endl;

    return false;
}

  size_t DatabaseRAM::NonDeletedNewsGroupCount() 
  {
    MapNewsGroup::iterator it;
    cout << "\tNG.size(): "<<newsgroups->size() << endl;
    size_t count = 0;
    for ( it = newsgroups->begin(); it != newsgroups->end(); ++it) {
      cout << "\t\t\t\t IsDeleted: " << it->second.IsDeleted() << endl;
      if (!it->second.IsDeleted()) {
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
        cout << "Checking ArticleCount() for a news group that does not exist. " << endl;
        return 0;
    }
}

bool DatabaseRAM::FindNewsGroup(string name) const
{
    // MapNewsGroup::const_iterator it = find_if(
    //         newsgroups->begin(),
    //         newsgroups->end(),
    //         bind2nd(FindNewsGroupByName(), name));

    // return it != newsgroups->end();
    return name.empty();
}
}
