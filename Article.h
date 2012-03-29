#ifndef ARTICLE_H
#define ARTICLE_H

#include <string>
#include <sstream>

using std::string;
using std::stringstream;

namespace usenet
{
class Article
{
    friend class NewsGroup;
public:
    Article(string title, string author, string text) :
        title(title), author(author), text(text), isDeleted(false) {}

    Article(string title, string author, string text, bool deleted) :
        title(title), author(author), text(text), isDeleted(deleted) {}

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
    bool IsDeleted()
    {
        return isDeleted;
    }

    string ToString()
    {
        stringstream ss;
        ss << "[" << title << ", " << author << ", " << text.substr(0, 10) << "...]:" << (isDeleted ? "deleted" : "");
        return ss.str();
    }

private:
    string title;
    string author;
    string text;
    bool isDeleted;
};
}

#endif
