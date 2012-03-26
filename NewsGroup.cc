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
    if (idx > articles.size() || idx == 0 || !ArticleExists(aID))
        return false;

    articles[aID].Delete();
    deletedArticles++;
    return articles[aID].IsDeleted();
}

Article const *const NewsGroup::GetArticle(int aID)
{
    Article *result = 0;
    if (ArticleExists(aID))
        result = &articles[static_cast<vector<Article>::size_type>(aID)];

    return result;
}

bool NewsGroup::ArticleExists(int aID) {
    return  static_cast<vector<Article>::size_type>(aID) < articles.size() &&
            !articles[static_cast<vector<Article>::size_type>(aID)].IsDeleted();
}

bool NewsGroup::FindArticle(string title) const {
    vector<Article>::const_iterator it = find_if(
        articles.begin(), 
        articles.end(), 
        bind2nd(FindArticleByName(), title));
    return it != articles.end();
}
}