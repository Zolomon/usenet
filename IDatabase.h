#ifndef IDATABASE_H
#define IDATABASE_H

#include "MessageHandler.h"
#include "NewsGroup.h"
#include "Article.h"
#include <string>
#include <vector>

using std::vector;
using std::string; 

namespace usenet
{
class IDatabase
{
public:
    virtual vector<NewsGroup> ListNewsGroups() = 0;
    virtual bool CreateNewsGroup(string title) = 0;
    virtual bool DeleteNewsGroup(int ngID) = 0;
    virtual vector<Article> ListArticles(int ngID) = 0;
    virtual bool CreateArticle(int ngID, string title, string author, string text) = 0;
    virtual bool DeleteArticle(int ngID, int aID) = 0;
    virtual Article const * const GetArticle(int ngID, int aID) = 0;
};
}
#endif