#ifndef DATABASERAM_H
#define DATABASERAM_H

#include "IDatabase.h"
#include "MessageHandler.h"

namespace usenet
{
class DatabaseRAM : public IDatabase
{
public:
    void ListNewsGroups(MessageHandler &mh) const;
    void CreateNewsGroup(MessageHandler &mh);
    void DeleteNewsGroup(MessageHandler &mh);

    void ListArticles(MessageHandler &mh) const;
    void CreateArticle(MessageHandler &mh);
    void DeleteArticle(MessageHandler &mh);
    void GetArticle(MessageHandler &mh) const;

private:

};
}

#endif