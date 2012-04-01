#include "NewsGroup.h"
#include <algorithm>
#include <utility>
#include <sstream>

using namespace std;

namespace usenet
{
typedef map<int, Article> MapArticle;

MapArticle *NewsGroup::ListArticles()
{
    MapArticle *amap = new MapArticle(articles->begin(), 
                                      articles->end());
    return amap;
}

void NewsGroup::CreateArticle(string title, string author, string text)
{
    articles->insert(make_pair(++NewsGroup::articleID, Article(title, author, text)));
}

bool NewsGroup::DeleteArticle(int aID)
{
    if (!ArticleExists(aID))
        return false;

    MapArticle::iterator it = articles->find(aID);

    bool deleted = false;
    if (it != articles->end() && !it->second.IsDeleted())
    {

        it->second.Delete();
        deletedArticles++;
        deleted = true;
    }
    return deleted;
}

Article const *NewsGroup::GetArticle(int aID)
{
    Article const *doesNotExist = 0;
    if (ArticleExists(aID))
    {
        MapArticle::iterator it = articles->find(aID);
        if (it != articles->end()) 
            return doesNotExist = new Article(it->second.GetTitle(),
                it->second.GetAuthor(),
                it->second.GetText());
    }

    return doesNotExist;
}

bool NewsGroup::ArticleExists(int aID)
{
    MapArticle::iterator it = articles->find(aID);

    return it != articles->end() && !it->second.IsDeleted();
}

size_t NewsGroup::NonDeletedArticleCount()
{
    MapArticle::iterator it;
    size_t count = 0;
    for (it = articles->begin(); it != articles->end(); ++it)
    {
        if (!it->second.IsDeleted())
            ++count;
    }
    return count;
}

string NewsGroup::ToString()
{
    stringstream ss;

    MapArticle::iterator it;

    ss << "NewsGroup: [" << name.substr(0, 10) << "]:" << (isDeleted ? "deleted" : "") << endl;
    for (it = articles->begin(); it != articles->end(); ++it)
    {
        ss << "\t" << it->first << ":" << it->second.ToString() << endl;
    }
    return ss.str();
}

bool NewsGroup::FindArticle(string title) const
{
    MapArticle::iterator it;
    for (it = articles->begin(); it != articles->end(); ++it)
    {
        if (it->second.GetTitle() == title)
            return true;
    }

    return false;
}
}
