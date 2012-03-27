#ifndef DATABASERAM_H
#define DATABASERAM_H

#include "IDatabase.h"
#include "MessageHandler.h"

namespace usenet
{
class DatabaseRAM : public IDatabase
{
public:
	typedef map<int, NewsGroup> MapNewsGroup;
	typedef map<int, Article> MapArticle;

	DatabaseRAM() { newsgroups = new MapNewsGroup(); deletedGroups = 0;}

	MapNewsGroup* ListNewsGroups();
	bool CreateNewsGroup(string title);
	bool DeleteNewsGroup(int ngID);
	bool NewsGroupExists(int ngID);
	size_t NonDeletedNewsGroupCount(); 

	MapArticle* ListArticles(int ngID);
	bool CreateArticle(int ngID, string title, string author, string text);
	bool DeleteArticle(int ngID, int aID);
	Article const * GetArticle(int ngID, int aID);
	bool ArticleExists(int ngID, int aID);
	size_t NonDeletedArticleCount(int ngID);

	string ToString();

private:
	bool FindNewsGroup(string name) const;
	MapNewsGroup* newsgroups;
	static int ID; // TODO: FULHACK, FIXA
	size_t deletedGroups;
};

struct FindNewsGroupByName : public binary_function<NewsGroup, string, bool>
{
	bool operator() (const NewsGroup &newsgroup, string name) const
	{
		bool found = name == newsgroup.GetName();
		if (found) cout << "NewsGroup("<<name<<") found!" << endl;
		else cout << "NewsGroup("<<name<<") NOT found!" << endl;
		return found;
	}
};
}

#endif
