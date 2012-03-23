#ifndef IDATABASE_H
#define IDATABASE_H

#include "MessageHandler.h"

namespace usenet
{
class IDatabase
{
public:
    virtual void ListNewsGroups(MessageHandler &mh) const = 0;
    virtual void CreateNewsGroup(MessageHandler &mh) = 0;
    virtual void DeleteNewsGroup(MessageHandler &mh) = 0;
    virtual void ListArticles(MessageHandler &mh) const = 0;
    virtual void CreateArticle(MessageHandler &mh) = 0;
    virtual void DeleteArticle(MessageHandler &mh) = 0;
    virtual void GetArticle(MessageHandler &mh) const = 0;
};
}
#endif