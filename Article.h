#ifndef ARTICLE_H
#define ARTICLE_H

#include <string>

using std::string;

namespace usenet
{
class Article
{
    friend class NewsGroup;
public:
    Article(string title, string author, string text) :
        title(title), author(author), text(text), isDeleted(false) {}

    string GetTitle() const
    {
        return title;
    }
    string GetAuthor() const 
    {
        return author;
    }
    string GetText() const 
    {
        return text;
    }
    void Delete()
    {
        isDeleted = true;
    }
    bool IsDeleted() {
        return isDeleted;
    }

private:
    string title;
    string author;
    string text;
    bool isDeleted;
};
}

#endif
