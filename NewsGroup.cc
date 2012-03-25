#include "NewsGroup.h"
namespace usenet
{
vector<Article> NewsGroup::ListArticles()
{
    return articles;
}

void NewsGroup::CreateArticle(string title, string author, string text)
{
    articles.push_back(Article(title, author, text));
}

bool NewsGroup::DeleteArticle(int aID)
{
    vector<Article>::size_type idx = static_cast<vector<Article>::size_type>(aID);
    if (idx > articles.size() || idx == 0)
        return false;

    if (articles[idx].isDeleted)
        return false;

    articles[aID].Delete();
    return articles[aID].isDeleted;
}

Article const *const NewsGroup::GetArticle(int aID)
{
    Article *result = 0;
    vector<Article>::size_type idx = static_cast<vector<Article>::size_type>(aID);
    if (idx < articles.size())
        result = &articles[idx];

    return result;
}

bool NewsGroup::FindArticle(string title) const {
    vector<Article>::const_iterator it = find_if(
        articles.begin(), 
        articles.end(), 
        bind2nd(FindArticleByName(), title));
    return it != articles.end();
}
}