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
    virtual vector<NewsGroup> 		ListNewsGroups() = 0;
    virtual bool 					CreateNewsGroup(string title) = 0;
    virtual void 					DeleteNewsGroup(int ngID) = 0;
    virtual bool					NewsGroupExists(int ngID) = 0;
    virtual size_t 					NewsGroupCount() = 0;
    
    virtual vector<Article> 		ListArticles(int ngID) = 0;
    virtual bool 					CreateArticle(int ngID, string title, string author, string text) = 0;
    virtual bool 					DeleteArticle(int ngID, int aID) = 0;
    virtual Article * 			 	GetArticle(int ngID, int aID) = 0;
    virtual bool 					ArticleExists(int ngID, int aID) = 0;
    virtual size_t 					ArticleCount(int ngID) = 0;
};
}
#endif