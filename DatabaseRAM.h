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
	~DatabaseRAM(); 
	
	MapNewsGroup* ListNewsGroups();
	bool CreateNewsGroup(string title);
	bool DeleteNewsGroup(int ngID);
	bool NewsGroupExists(int ngID);
	bool NewsGroupExists(string title);
	size_t NonDeletedNewsGroupCount(); 

	MapArticle* ListArticles(int ngID);
	bool CreateArticle(int ngID, string title, string author, string text);
	bool DeleteArticle(int ngID, int aID);
	Article const * GetArticle(int ngID, int aID);
	bool ArticleExists(int ngID, int aID);
	size_t NonDeletedArticleCount(int ngID);

	string ToString();

private:
	MapNewsGroup* newsgroups;
	static int ID;
	size_t deletedGroups;
};
}

#endif
