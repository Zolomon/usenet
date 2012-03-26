#ifndef DATABASERAM_H
#define DATABASERAM_H

#include "IDatabase.h"
#include "MessageHandler.h"

namespace usenet
{
class DatabaseRAM : public IDatabase
{
public:
	vector<NewsGroup> 			ListNewsGroups();
	bool 						CreateNewsGroup(string title);
	void 						DeleteNewsGroup(int ngID);
	bool						NewsGroupExists(int ngID);

	vector<Article>				ListArticles(int ngID);
	bool						CreateArticle(int ngID, string title, string author, string text);
	bool						DeleteArticle(int ngID, int aID);
	Article * 					GetArticle(int ngID, int aID);
	bool 						ArticleExists(int ngID, int aID);
	size_t 						NewsGroupCount() { return newsgroups.size()-deletedGroups; }
	size_t 						ArticleCount(int ngID) { return newsgroups[static_cast<vector<Article>::size_type>(ngID)].ArticleCount(); }
private:
	bool 					FindNewsGroup(string name) const;
	vector<NewsGroup> 		newsgroups;
	size_t 					deletedGroups;
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