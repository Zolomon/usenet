#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <string>
#include "Article.h"
#include <functional>
#include <iostream>
#include <map>

using std::binary_function;
using std::string;
using std::cout;
using std::endl;
using std::map;

namespace usenet
{
class NewsGroup
{
public:
    typedef map<int, Article> MapArticle;

    NewsGroup(string name, bool isDeleted) : name(name), isDeleted(isDeleted) {
        articleID = 0;
        articles = new MapArticle();
        deletedArticles = 0;
    }

    NewsGroup(string name) : name(name), isDeleted(false)
    {
        articleID = 0;
        articles = new MapArticle();
        deletedArticles = 0;
    }

    MapArticle *ListArticles();
    void CreateArticle(string title, string author, string text);
    bool DeleteArticle(int aID);
    Article const *GetArticle(int aID);
    bool ArticleExists(int aID);
    bool FindArticle(string title) const;
    size_t NonDeletedArticleCount();

    void Delete()
    {
        isDeleted = true;
    }
    bool IsDeleted() const
    {
        return isDeleted;
    }
    string GetName() const
    {
        return name;
    }

    MapArticle::size_type Size();

    string ToString();
private:
    friend class DatabaseRAM;
    string name;
    bool isDeleted;
    size_t deletedArticles;
    MapArticle *articles;
    static int articleID;
};
}

#endif
