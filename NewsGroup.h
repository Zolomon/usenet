#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <vector>
#include <string>
#include "Article.h"
#include <functional>

using std::binary_function;
using std::vector;
using std::string;

namespace usenet
{
class NewsGroup
{
public:
    NewsGroup(string name) : name(name), isDeleted(false) {}

    vector<Article> ListArticles();
    void CreateArticle(string title, string author, string text);
    bool DeleteArticle(int aID);
    Article const *const GetArticle(int aID);
    bool FindArticle(string title) const;
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

    vector<Article>::size_type Size();
private:
    friend class DatabaseRAM;
    string name;
    bool isDeleted;
    vector<Article> articles;
};

struct FindArticleByName : public binary_function<Article, string, bool>
{
    bool operator() (const Article &article, string title) const
    {
        return title == article.GetTitle();
    }
};
}

#endif