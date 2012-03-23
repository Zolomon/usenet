#include "DatabaseRAM.h"
#include "MessageHandler.h"
#include <iostream>
#include "protocol.h"

using std::cout;
using std::endl;
using std::cerr;
using protocol::Protocol;

namespace usenet
{
void DatabaseRAM::ListNewsGroups(MessageHandler &mh) const {
	mh.TOREMOVE("ListNewsGroupsRAM");

	int code = mh.recvCode();
    cout << "recvCode: " << code << endl;
    if (code != Protocol::COM_END)
        cerr << "Malformed command...";

    cout << "Listing all NewsGroups..." << endl;
    mh.sendCode(Protocol::ANS_LIST_NG);
    mh.sendIntParameter(1);
    mh.sendIntParameter(25);
    mh.sendStringParameter("string param");
    mh.sendCode(Protocol::ANS_END);
}
void DatabaseRAM::CreateNewsGroup(MessageHandler &mh) {
	mh.TOREMOVE("CreateNewsGroupRAM");
}
void DatabaseRAM::DeleteNewsGroup(MessageHandler &mh) {
	mh.TOREMOVE("DeleteNewsGroupRAM");
}
void DatabaseRAM::ListArticles(MessageHandler &mh) const {
	mh.TOREMOVE("ListArticlesRAM");
}
void DatabaseRAM::CreateArticle(MessageHandler &mh) {
	mh.TOREMOVE("CreateArticleRAM");
}
void DatabaseRAM::DeleteArticle(MessageHandler &mh) {
	mh.TOREMOVE("DeleteArticleRAM");
}
void DatabaseRAM::GetArticle(MessageHandler &mh) const {
	mh.TOREMOVE("GetArticleRAM");
}
}