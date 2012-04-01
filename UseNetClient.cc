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
  int i = 0;
  cout << "Newsgroup ID: Newsgroup name" << endl;
  for (i = 0; i < nbr; ++i) {
    int ngID = mh.recvIntParameter();
    string name = mh.recvStringParameter();
    cout << ngID << ": " << name << endl;
  }

  int END =  mh.recvCode();
  if(END != Protocol::ANS_END) 
    cerr << "Malformed command: " << END << ", expected: " << Protocol::ANS_END << endl;
}

void HandleCreateNewsGroup(MessageHandler &mh)
{
  mh.sendCode(Protocol::COM_CREATE_NG);
  cout << "Enter NewsGroup name" << endl;
  string name;
  cin >> name;
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
  mh.sendCode(Protocol::COM_DELETE_NG);
  int ngID;
  cout << "Enter a newsgroup ID:" << endl;
  cin >> ngID;
  mh.sendIntParameter(ngID);
  mh.sendCode(Protocol::COM_END);
  int code;
  code = mh.recvCode();
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
  mh.sendCode(Protocol::COM_LIST_ART);
  int ngID;
  cout << "Enter a newsgroup ID:" << endl;
  cin >> ngID;
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
    int i = 0;
	cout << "Artricle ID: Article name" << endl;
    for(i = 0; i < nbrArt; ++i){
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
  cin >> ngID;
  cout << "Enter a title:" << endl;
  cin >> title;
  cout << "Enter a author name:" << endl;
  cin >> author;
  cout << "Enter article text:" << endl;
  cin >> text;
	
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
  mh.sendCode(Protocol::COM_DELETE_ART);
  int ngID;
  int artID;
  cout << "Enter a newsgroup ID:" << endl;
  cin >> ngID;
  cout << "Enter an article ID:" << endl;
  cin >> artID;
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
  mh.sendCode(Protocol::COM_GET_ART);
  int ngID;
  int artID;
  cout << "Enter a newsgroup ID:" << endl;
  cin >> ngID;
  cout << "Enter an article ID:" << endl;
  cin >> artID;
  mh.sendIntParameter(ngID);
  mh.sendIntParameter(artID);
  mh.sendCode(Protocol::COM_END);
	
  int code = mh.recvCode();
  if(code != Protocol::ANS_GET_ART){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_END << endl;
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
  if(code == Protocol::ANS_END){
    cerr << "Malformed command: " << code << ", expected: " << Protocol::ANS_END << endl;
  }
}

int main()
{

  cout << "Enter host name" << endl;
  string hostname;
  cin >> hostname;
  cout << "Enter port name" << endl;
  int portname;
  cin >> portname;
  Connection conn(hostname.c_str(), portname);
  
  if(!conn.isConnected()) {
    cerr << "Connection attempt failed" << endl;
    exit(1);
  }
  try {
    MessageHandler mh(&conn);
    while(conn.isConnected()) {
      cout << "Enter command" << endl;
      int command;
      cin >> command;

      switch (command) 
	{
	case Protocol::COM_LIST_NG:
	  {
	    HandleListNewsGroups(mh);
	  }
	  break;
	case Protocol::COM_CREATE_NG:
	  {
	    HandleCreateNewsGroup(mh);
	  }
	  break;
	case Protocol::COM_DELETE_NG:
	  {
	    HandleDeleteNewsGroup(mh);
	  }
	  break;
	case Protocol::COM_LIST_ART:
	  {
	    HandleListArticles(mh);
	  }
	  break;
	case Protocol::COM_CREATE_ART:
	  {
	    HandleCreateArticle(mh);
	  }
	  break;
	case Protocol::COM_DELETE_ART:
	  {
	    HandleDeleteArticle(mh);
	  }
	  break;
	case Protocol::COM_GET_ART:
	  {
	    HandleGetArticle(mh);
	  }
	  break;
	default:
	  {
	    cerr << "Unexpected command" << endl;
	    exit(1);
	  }
	  break;
	}
    }
  } catch (ConnectionClosedException &){
    cerr << "Connection closed" << endl;
  }
}
