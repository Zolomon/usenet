#ifndef DATABASEDB_H
#define DATABASEDB_H

#include "IDatabase.h"

namespace usenet
{
class DatabaseDB : public IDatabase
{
public:
    void ListNewsGroups(MessageHandler &mh) const;
    void CreateNewsGroup(MessageHandler &mh);
    void DeleteNewsGroup(MessageHandler &mh);
    
    void ListArticles(MessageHandler &mh) const;
    void CreateArticle(MessageHandler &mh);
    void DeleteArticle(MessageHandler &mh);
    void GetArticle(MessageHandler &mh) const;
};
}

#endif