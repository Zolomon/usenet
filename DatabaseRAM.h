#ifndef DATABASERAM_H
#define DATABASERAM_H

#include "IDatabase.h"
#include "MessageHandler.h"

namespace usenet
{
class DatabaseRAM : public IDatabase
{
public:
    vector<NewsGroup> ListNewsGroups();
    bool CreateNewsGroup(string title);
    bool DeleteNewsGroup(int ngID);
    vector<Article> ListArticles(int ngID);
    bool CreateArticle(int ngID, string title, string author, string text);
    bool DeleteArticle(int ngID, int aID);
    Article const *const GetArticle(int ngID, int aID);
private:
	bool FindNewsGroup(string name) const;
    vector<NewsGroup> newsgroups;
};
struct FindNewsGroupByName : public binary_function<NewsGroup, string, bool>
{
    bool operator() (const NewsGroup &newsgroup, string title) const
    {
        return title == newsgroup.GetName();
    }
};
}

#endif