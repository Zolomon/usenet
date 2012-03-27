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
	typedef map<int, NewsGroup> MapNewsGroup;
	typedef map<int, Article> MapArticle;

    virtual MapNewsGroup* ListNewsGroups() = 0;
    virtual bool CreateNewsGroup(string title) = 0;
    virtual bool DeleteNewsGroup(int ngID) = 0;
    virtual bool NewsGroupExists(int ngID) = 0;
    virtual size_t NonDeletedNewsGroupCount() = 0;
    
    virtual MapArticle* ListArticles(int ngID) = 0;
    virtual bool CreateArticle(int ngID, string title, string author, string text) = 0;
    virtual bool DeleteArticle(int ngID, int aID) = 0;
    virtual Article const * GetArticle(int ngID, int aID) = 0;
    virtual bool ArticleExists(int ngID, int aID) = 0;
    virtual size_t NonDeletedArticleCount(int ngID) = 0;
    virtual string ToString() = 0;
};
}
#endif
