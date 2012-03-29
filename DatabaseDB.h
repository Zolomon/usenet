#ifndef DATABASEDB_H
#define DATABASEDB_H

#include "IDatabase.h"
#include "sqlite/sqlite3.h"

namespace usenet
{
class DatabaseDB : public IDatabase
{
	public:
	typedef map<int, NewsGroup> MapNewsGroup;
	typedef map<int, Article> MapArticle;

	DatabaseDB();
	~DatabaseDB();

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
	sqlite3 *db;

	// Database functions
	bool TableExists(string tablename);
	void FillDatabase();
	void CreateNewsGroupTable();
	void CreateArticleTable();
};
}

#endif