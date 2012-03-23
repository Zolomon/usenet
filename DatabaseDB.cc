#include "DatabaseDB.h"
#include "MessageHandler.h"

namespace usenet
{
void DatabaseDB::ListNewsGroups(MessageHandler &mh) const {
	mh.TOREMOVE("ListNewsGroupsDB");
}
void DatabaseDB::CreateNewsGroup(MessageHandler &mh) {
	mh.TOREMOVE("CreateNewsGroupDB");
}
void DatabaseDB::DeleteNewsGroup(MessageHandler &mh) {
	mh.TOREMOVE("DeleteNewsGroupDB");
}
void DatabaseDB::ListArticles(MessageHandler &mh) const {
	mh.TOREMOVE("ListArticlesDB");
}
void DatabaseDB::CreateArticle(MessageHandler &mh) {
	mh.TOREMOVE("CreateArticleDB");
}
void DatabaseDB::DeleteArticle(MessageHandler &mh) {
	mh.TOREMOVE("DeleteArticleDB");
}
void DatabaseDB::GetArticle(MessageHandler &mh) const {
	mh.TOREMOVE("GetArticleDB");
}
}