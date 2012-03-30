#include "connection.h"
#include "protocol.h"
#include "MessageHandler.h"
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;
using protocol::Protocol;
using namespace usenet;

void HandleListNewsGroups(MessageHandler &mh) 
{
  mh.sendCode(Protocol::COM_LIST_NG);
  mh.sendCode(Protocol::COM_END);
  int ansListNg = mh.recvCode();
  if(ansListNg != Protocol::ANS_LIST_NG) 
    cerr << "Malformed command: " << ansListNg << ", expected: " << Protocol::ANS_LIST_NG << endl;
  int nbr = mh.recvIntParameter();
  if(nbr <= 0 ) {
    cout << "No Newsgroups exists yet. Be the first to create one!" << endl;
    return;
  } else { 
  cout << "Newsgroup ID: Newsgroup name" << endl;
  }
  for (int i = 0; i < nbr; ++i) {
    int ngID = mh.recvIntParameter();
    string name = mh.recvStringParameter();
    cout << ngID << ": " << name << endl;
  }
  cout << "fuuu" << endl;
  int END =  mh.recvCode();
  if(END != Protocol::ANS_END) 
    cerr << "Malformed command: " << END << ", expected: " << Protocol::ANS_END << endl;
}

void HandleCreateNewsGroup(MessageHandler &mh)
{
  mh.sendCode(Protocol::COM_CREATE_NG);
  string name;
  cout << "Enter NewsGroup name" << endl;
  getline(cin,name);
  mh.sendStringParameter(name);
  mh.sendCode(Protocol::COM_END);
  mh.recvCode();
  int code = mh.recvCode();
  if(code==Protocol::ANS_ACK) {
    cout << "NewsGroup created with name: " << name << endl;
  } else if (code==Protocol::ANS_NAK) {
    if(mh.recvCode() == Protocol::ERR_NG_ALREADY_EXISTS) {
      cerr << "NewsGroup with that name already exists" << endl;
    } else {
      cerr << "Something is wrong!" << endl;
    }
  }
  if(mh.recvCode() != Protocol::ANS_END) 
    cerr << "Protocol error" << endl;
}

void HandleDeleteNewsGroup(MessageHandler &mh) 
{
  string temp;
  mh.sendCode(Protocol::COM_DELETE_NG);
  cout << "Enter a newsgroup ID:" << endl;
  getline(cin,temp);
  int ngID = atoi(temp.c_str());
  mh.sendIntParameter(ngID);
  mh.sendCode(Protocol::COM_END);
  int code = mh.recvCode();
  if(code != Protocol::ANS_DELETE_NG) {
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_DELETE_NG << endl;
  }
  code = mh.recvCode();
  if(code == Protocol::ANS_ACK) {
    cout << "Newsgroup removed" << endl;
  } else if(code == Protocol::ANS_NAK) {
    code = mh.recvCode();
    if(code == Protocol::ERR_NG_DOES_NOT_EXIST){
      cout << "Newgroup does not exist." << endl;
    }
  }
  code = mh.recvCode();
  if(code != Protocol::ANS_END){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_END << endl;
  }
}

void HandleListArticles(MessageHandler &mh){
  string temp;
  mh.sendCode(Protocol::COM_LIST_ART);
  cout << "Enter a newsgroup ID:" << endl;
  getline(cin,temp);
  int ngID = atoi(temp.c_str());
  mh.sendIntParameter(ngID);
  mh.sendCode(Protocol::COM_END);
  int code;
  code = mh.recvCode();
  if(code != Protocol::ANS_LIST_ART){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_LIST_ART << endl;
  }
  code = mh.recvCode();
  if(code == Protocol::ANS_ACK){
    int nbrArt = mh.recvIntParameter();
    if(nbrArt <= 0 ) {
      cout << "This newsgroup contains no articles, be the first to write one!" << endl;
      return;
    }
    cout << "Artricle ID: Article name" << endl;
    for(int i = 0; i < nbrArt; ++i){
      int artID = mh.recvIntParameter();
      string artName = mh.recvStringParameter();
      cout << artID << ": " << artName << endl;
    }
  } else {
    code = mh.recvCode();
    code = mh.recvCode();
    if(code == Protocol::ERR_NG_DOES_NOT_EXIST){
      cout << "Newsgroup does not exist" << endl;
    }
  }
  code = mh.recvCode();
  if(code != Protocol::ANS_END){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_END << endl;
  }
}

void HandleCreateArticle(MessageHandler &mh){
  mh.sendCode(Protocol::COM_CREATE_ART);
  int ngID;
  string title;
  string author;
  string text;
  cout << "Enter a newgroup ID:" << endl;
  string tempID;
  getline(cin, tempID);
  ngID = atoi(tempID.c_str());
  cout << "Enter a title:" << endl;
  getline(cin,title);
  cout << "Enter a author name:" << endl;
  getline(cin, author);
  cout << "Enter article text:" << endl;
  getline(cin, text);

  mh.sendIntParameter(ngID);
  mh.sendStringParameter(title);
  mh.sendStringParameter(author);
  mh.sendStringParameter(text);
  mh.sendCode(Protocol::COM_END);
  int code = mh.recvCode();
  if(code != Protocol::ANS_CREATE_ART){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_CREATE_ART << endl;
  }
  code = mh.recvCode();
  if(code == Protocol::ANS_ACK){
    cout << "Article \"" << title << "\" added." << endl;
  } else if (code == Protocol::ANS_NAK){
    code = mh.recvCode();
    if(code == Protocol::ERR_NG_DOES_NOT_EXIST){
      cout << "Newsgroup " << ngID << " does not exist." << endl;
    }
  }
  code = mh.recvCode();
  if(code != Protocol::ANS_END){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_END << endl;
  }
}	

void HandleDeleteArticle(MessageHandler &mh){
  string temp;
  mh.sendCode(Protocol::COM_DELETE_ART);
  cout << "Enter a newsgroup ID:" << endl;
  getline(cin,temp);
  int ngID = atoi(temp.c_str());
  cout << "Enter an article ID:" << endl;
  getline(cin,temp);
  int artID = atoi(temp.c_str());
  mh.sendIntParameter(ngID);
  mh.sendIntParameter(artID);
  mh.sendCode(Protocol::COM_END);

  int code = mh.recvCode();
  if(code != Protocol::ANS_DELETE_ART){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_DELETE_ART << endl;
  }
  code = mh.recvCode();
  if(code == Protocol::ANS_ACK){
    cout << "Article deleted" << endl;
  } else if (code == Protocol::ANS_NAK){
    code = mh.recvCode();
    if(code == Protocol::ERR_ART_DOES_NOT_EXIST){
      cout << "Article does not exist" << endl;
    } else if (code == Protocol::ERR_NG_DOES_NOT_EXIST){
      cout << "Newsgroup does not exist" << endl;
    }
  }
  code = mh.recvCode();
  if(code != Protocol::ANS_END){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_END << endl;
  }
}

void HandleGetArticle(MessageHandler &mh){
  string temp;
  mh.sendCode(Protocol::COM_GET_ART);
  cout << "Enter a newsgroup ID:" << endl;
  getline(cin,temp);
  int ngID = atoi(temp.c_str());
  cout << "Enter an article ID:" << endl;
  getline(cin,temp);
  int artID = atoi(temp.c_str());
  mh.sendIntParameter(ngID);
  mh.sendIntParameter(artID);
  mh.sendCode(Protocol::COM_END);

  int code = mh.recvCode();
  if(code != Protocol::ANS_GET_ART){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_GET_ART << endl;
  }
  code = mh.recvCode();
  if(code == Protocol::ANS_ACK){	
    string title = mh.recvStringParameter();
    string author = mh.recvStringParameter();
    string text = mh.recvStringParameter();
    cout << title << endl;
    cout << author << endl;
    cout << text << endl;
  } else if (code == Protocol::ANS_NAK){
    code = mh.recvCode();
    if(code == Protocol::ERR_NG_DOES_NOT_EXIST){
      cout << "Newsgroup does not exist" << endl;
    } else if (code == Protocol::ERR_ART_DOES_NOT_EXIST){
      cout << "Article does not exist" << endl;
    }
  }
  code = mh.recvCode();
  if(code != Protocol::ANS_END){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_END << endl;
  }
}

int main()
{
  string temp;
  // cout << "Enter host name" << endl;
  string hostname = "localhost";
  cout << "Enter port name" << endl;
  getline(cin,temp);
  int portname = atoi(temp.c_str());
  Connection conn(hostname.c_str(), portname);
  
  if(!conn.isConnected()) {
    cerr << "Connection attempt failed" << endl;
    exit(1);
  }
  try {
    MessageHandler mh(&conn);
    while(conn.isConnected()) {
      cout << "Enter command" << endl;
      cout << "1. List newsgroups" << endl;
      cout << "2. Create newsgroup" << endl;
      cout << "3. Delete newsgroups" << endl;
      cout << "4. List articles" << endl;
      cout << "5. Read article" << endl;     
      cout << "6. Create article" << endl;
      cout << "7. Delete article" << endl;
      cout << "0. Disconnect" << endl;
     
      int command;
      getline(cin, temp);
      command = atoi(temp.c_str());
      switch (command) 
	{
	case 1:
	  {
	    HandleListNewsGroups(mh);
	  }
	  break;
	case 2:
	  {
	    HandleCreateNewsGroup(mh);
	  }
	  break;
	case 3:
	  {
	    HandleDeleteNewsGroup(mh);
	  }
	  break;
	case 4:
	  {
	    HandleListArticles(mh);
	  }
	  break;
	case 5:
	  {
	    HandleGetArticle(mh);
	   
	  }
	  break;
	case 6:
	  {
	    HandleCreateArticle(mh);
	   
	  }
	  break;
	case 7:
	  {
	    HandleDeleteArticle(mh);
	  }
	  break;
	case 0:
	  {
	    conn.~Connection();
	    exit(0);
	  }
	default:
	  {
	    cerr << "Unexpected command: " << command << endl;
	    conn.~Connection();
	    exit(1);
	  }
	  break;
	}
    }
  } catch (ConnectionClosedException &){
    cerr << "Connection closed" << endl;
  }
}
