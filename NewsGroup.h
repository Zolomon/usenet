#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <vector>
#include <string>
#include "Article.h"
#include <functional>
#include <iostream>
#include <algorithm>

using std::binary_function;
using std::vector;
using std::string;
using std::cout;
using std::endl;

namespace usenet
{
class NewsGroup
{
public:
    NewsGroup(string name) : name(name), isDeleted(false) {}

    vector<Article>                 ListArticles();
    void                            CreateArticle(string title, string author, string text);
    bool                            DeleteArticle(int aID);
    Article *                       GetArticle(int aID);
    bool                            ArticleExists(int aID);
    bool                            FindArticle(string title) const;
    size_t                          ArticleCount() {return articles.size() - deletedArticles;}

    void                            Delete() { isDeleted = true; }
    bool                            IsDeleted() const { return isDeleted; }
    string                          GetName() const { return name; }

    vector<Article>::size_type      Size();
private:
    friend class DatabaseRAM;
    string                          name;
    bool                            isDeleted;
    size_t                          deletedArticles;
    vector<Article>                 articles;
};

struct FindArticleByName : public binary_function<Article, string, bool>
{
    bool operator() (const Article &article, string title) const
    {
        bool found = title == article.GetTitle();
        if (found) cout << "Found article! T: " << title  << endl;
        else cout << "Couldn't find Article! T: " << title << endl;
        return found;
    }
};
}

#endif
