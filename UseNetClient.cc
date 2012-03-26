#include "connection.h"
#include "protocol.h"

using namespace std;
using protocol::Protocol;

void HandleListNewsGroups(MessageHandler &mh) {
  mh.sendCode(Protocol::COM_END);
  int ansListNg = mh.recvCode();
  if(ansListNg != Protocol::ANS_LIST_NG) 
    cerr << "Malformed command: " << ansListNg << ", expected: " << Protocol::ANS_LIST_NG << endl;
  int nbr = mh.recvInt();
  int i = 0;
  for (i; i < nbr; ++i) {
    int nbrChars = mh.recvIntParameter();
    string name = mh.recvStringParameter();
    cout << name << endl;
  }

int END =  mh.recvCode();
 if(END != Protocol::ANS_END) 
    cerr << "Malformed command: " << END << ", expected: " << Protocol::ANS_END << endl;
}

void HandleCreateNewsGroup(MessageHandler &mh)
{
  cout << "Enter NewsGroup name" << endl;
  string name;
  cin >> name;
  mh.sendStringParameter(name);
  mh.sendCode(COM_END);
  mh.recvCode();
  int code = mh.recvCode();
  if(code==Protocol::ANS_ACK) {
    cout << "NewsGroup created with name: " << name << endl;
  } else if (code==Protocol::ANS_NAK) {
    if(mh.recvCode() == Protocol::ERR_NG_ALREADY_EXIST) {
      cerr << "NewsGroup with that name already exists" << endl;
    } else {
	cerr << "Something is wrong!" << endl;
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
  Connection conn(hostname, portname);
  
  if(!conn.isConnected()) {
    cerr << "Connection attempt failed" << endl;
    exit(1);
  }
  try {
    MessageHandler mh(conn);
    while(conn.isConnected()) {
      cout << "Enter command" << endl;
      int command;
      cin >> command;

      switch (command) 
	{
	case  case Protocol::COM_LIST_NG:
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
	}
    
    }
