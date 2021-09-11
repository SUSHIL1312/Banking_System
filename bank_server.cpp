/*
				Assignment No:03 (Banking system using client-server Socket Programming)
									[Server Program]
										[Group 13]
		members:  Shubham................................. 204101054
				  Stuti Priyambda........................ 204101055
				  Subham Das............................ 204101056
				  Sushil Kumar............................ 204101057
*/
/*
		Execution: compile and run in Terminal. 
		for server :  g++ bank_server.cpp -o bank_server
					  ./bank_server <port_no>

*/
//including all the necessary library & files.


#include<iostream>
using namespace std;
#include <string.h>
#include<cstring>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include <fcntl.h> 
#include <unistd.h> 
#include<string.h>
#include<fstream>
#include<vector>
#include<sstream>
#include <arpa/inet.h> 
#include<unordered_set>

#define USER 0
#define POLICE 1
#define ADMIN 2
#define UNAUTH_USER -1
#define RESPONSE_BYTES 512
#define REQUEST_BYTES 512
#define linesInMS 5
#define EXIT -1

//function declaration of the functions used in this program.
void sendMsgtoClient(int clientFD,string str);
string recieveMsgFromClient(int clientFD);
string getMiniStatement(string username,int client_fd);
void customerRequests(string username,string password,int clientFD);
int check_if_customer(string user);
void updateCustomerDatabase(string username,int choice,double balance);
int adminUtility(string username,int clientFD);
void adminRequests(int clientFD);
string getAllCustomerBalance();
void policeRequests(int clientFD);
int authorize(string username,string password);
void closeclient(int clientFD,string str);
void bankClientHandler(int clientFD);
string getBalance(string username);

unordered_set <string> allCustomers({"101","102","103","104","105","106",
"107","108","109","110"});

void error(const char *msg){
    perror(msg);
    exit(1);
}

/*
	customerRequests() is for the customers, i.e it helps the customers
	in checking ministatment and balance in his own account.
*/
void customerRequests(string username,string password,int clientFD){
    
    sendMsgtoClient(clientFD,"Enter your choice\n1. Available Balance\n2. Mini Statement\n3. for quitting.");
    string buffer="";
    while(1){
        buffer=recieveMsgFromClient(clientFD);//msg from receiver
        int choice=stoi(buffer);//converting string to integer
        string balance="";
        string str="";
        balance+=".........\nAvailable Balance: ";
        str+="............\nMini Statement:\n";
        if(choice==1){
            balance+=getBalance(username);
            balance+="\n------------------\n\nEnter your choice\n1. Available Balance\n2. Mini Statement\n3. for quitting.";
            sendMsgtoClient(clientFD,balance);
        }else if(choice==2){
            str+=getMiniStatement(username,clientFD);
            str+="\n------------------\n\nEnter your choice\n1. Available Balance\n2. Mini Statement\n3. for quitting.";
			sendMsgtoClient(clientFD,str);
		    str.clear();
        }else if(choice==3){
            break;
        }else{
            sendMsgtoClient(clientFD,"Unknown query");

        }
    }

}
/*
	check_if_customer() will help in identifying whether the user 
	who have logged in bank system is a Customer of bank or not.
	If he is a bank customer then it returns 1 otherwise(i.e for 
	admin ,police and customer not exist in bank).
*/
int check_if_customer(string user){
    
    ifstream file("login_file");
    if (file.is_open()) {
        string line;
        while (std::getline(file, line)) {
            
             vector <string> words;
            string word = ""; 
            for (auto x : line)  
            { 
                if (x == ' ') 
                { 
                    words.push_back(word);
                    word = ""; 
                } 
                else { 
                    word = word + x; 
                } 
            } words.push_back(word);
            if(words[2]=="C"){
                file.close();
                return 1;
            }
        }
    }
    file.close();
    return 0;
}

/*
	recieveMsgFromClient() will receive all msges from the client.
	with the help of read()
*/
string recieveMsgFromClient(int clientFD) {

    int numPacketsToReceive = 0;
    int n = read(clientFD, &numPacketsToReceive, sizeof(int));
    if(n <= 0) {
        shutdown(clientFD, SHUT_WR);
        return NULL;
    }
    char *str = (char*)malloc(numPacketsToReceive*REQUEST_BYTES);
    memset(str, 0, numPacketsToReceive*REQUEST_BYTES);//clearing 
    char *str_p = str;
    int i;
    for(i = 0; i < numPacketsToReceive; ++i) {
        int n = read(clientFD, str, REQUEST_BYTES);
        str = str+REQUEST_BYTES;
    }
	string st(str_p);
    return st;
}
/*
	sendMsgtoClient() will send all the msg to the client over the network 
	using the write().
*/
void sendMsgtoClient(int clientFD, string strng) {
    char str[10024];
    strcpy(str,strng.c_str());
    int numPacketsToSend = (strlen(str)-1)/RESPONSE_BYTES + 1;
    int n = write(clientFD, &numPacketsToSend, sizeof(int));
    char *msgToSend = (char*)malloc(numPacketsToSend*RESPONSE_BYTES);
    strcpy(msgToSend, str);
    int i;
    for(i = 0; i < numPacketsToSend; ++i) {
        int n = write(clientFD, msgToSend, RESPONSE_BYTES);
        msgToSend += RESPONSE_BYTES;
    }
}


/*
	authorize() will check all the person /user of the banking system
	and detect wheter he is a bank customer , admin ,or police.
*/
int authorize(string username,string password){
    cout<<"Authorizing user :  "<<username<<endl;
	
    ifstream file("login_file");
    if (file.is_open()) {
        string line;
	 	
        while (std::getline(file, line)) {
            
             vector <string> words;
            string word = ""; 
            for (auto x : line)  
            { 
                if (x == ' ') 
                { 
                    words.push_back(word);
                    word = ""; 
                } 
                else { 
                    word = word + x; 
                } 
            } words.push_back(word);
            if(username==words[0]){
                if(password==words[1]){
                    if(words[2]=="C"){
                        return USER;
                    }else if(words[2]=="A"){
                        return ADMIN;
                    }else if(words[2]=="P"){
                        return POLICE;
                    }
                }
            }
        }
    }else{
		error("opening file");
	}
    file.close();
    return UNAUTH_USER;
}
/*
	bankClientHandler() is the fns called form main() and helps in 
	achieving all the functionality required in banking system 
	for different category of users .
*/
void bankClientHandler(int clientFD){
    string username="";
    string password="";
    int utype;
    
    //asking client for username
    sendMsgtoClient(clientFD,"Enter Username: ");
    username=recieveMsgFromClient(clientFD);
    //asking client for his password
    sendMsgtoClient(clientFD,"Enter Password: ");
	password=recieveMsgFromClient(clientFD);
    //authorizing the type of user
    utype=authorize(username,password);
    string tempString="Thanks!! Have a good day. ";
    tempString=tempString+username;
    switch(utype){
        case USER:{
            customerRequests(username,password,clientFD);
			closeclient(clientFD,tempString);
			break;
        }
        case ADMIN:
			adminRequests(clientFD);
			closeclient(clientFD,tempString);
			break;	
		case POLICE:
			policeRequests(clientFD);
			closeclient(clientFD,tempString);
			break;	
		case UNAUTH_USER:
			closeclient(clientFD,"unauthorised");
			break;
		default:
			closeclient(clientFD,"unauthorised");
			break;
    }

}
/*
	getMiniStatement() helps in getting the latest 5-transaction
	done by a customer from the bank.
*/
string getMiniStatement(string username,int client_fd)
{
	ifstream fin(username);

	string miniStatement="";

   	string line="";
    int count=0;

	while(count<linesInMS)
	{
		getline(fin,line);
		miniStatement+=line;
		miniStatement+="\n";
		count++;
	}

	fin.close();

	if(miniStatement.size()==0)
		miniStatement = "None";

	return miniStatement;
}
/*
		getBalance() helps in getting the balance of any customer.
*/
string getBalance(string username)
{
	ifstream fin(username);
	string line,balance;

    if(getline(fin,line))
    {
    	
		vector<string> token;
		stringstream tline(line); 
      
    	string tmp; 
      
    	while(getline(tline, tmp , ' ')) 
    	{ 
       		token.push_back(tmp); 
   	} 
    	
    	balance = token[6];
    }
    else
    {
    	balance = "0\n";
    }
    fin.close();
    return balance;
}
/*
	updateCustomerDatabase() helps in updating the database  of any customer of the
	banking system.
*/
void updateCustomerDatabase(string username,int choice,double balance)
{
	ifstream fin(username);
	string line="",tmp="";
	char c=(choice==1)?'C':'D';
	//getting the current time and date.
	time_t now = time(0);
   	string timestamp = ctime(&now);


	line += timestamp.substr(0,timestamp.size()-1);
	line = line + " " + c + " " + to_string(balance);
	while(fin)
	{
		getline(fin,tmp);
		line+='\n';
		line+=tmp;
	}

	fin.close();
	fstream fout(username);
	fout<<line;
	fout.close();
}
/*
	adminUtility() helps in acheiving the fucntionality of the admin
	of the banking system i.e he will able to credit or debit 
	amount to any customer of the bank.
*/
int adminUtility(string username, int client_fd)
{
	int flag=1;
	if(allCustomers.find(username)==allCustomers.end())
		{
			sendMsgtoClient(client_fd,"Entered Customer does not have account in this bank.\n");
			return EXIT;
		}
	sendMsgtoClient(client_fd,"Choose an option\n1. Credit\n2. Debit\nWrite exit to terminate");
	while(flag)
	{
		string buffer = recieveMsgFromClient(client_fd);

		

		if(buffer == "exit")
			return EXIT;
		else
		{
			int choice = stoi(buffer);
			double balance=stod(getBalance(username));

			if(choice!=1 && choice!=2)
				sendMsgtoClient(client_fd,"Unknown query.");
			else
			{
				sendMsgtoClient(client_fd,"Enter amount :");

				while(1)
				{
					string buffer = recieveMsgFromClient(client_fd);
					double amount=stod(buffer);
					
					if(amount<=0)
						sendMsgtoClient(client_fd,"Enter valid amount");
					else
					{
						if(choice==2 && balance<amount)
						{
							sendMsgtoClient(client_fd,"Insufficient Balance.\n--------------------\n\nEnter username of the account holder or 'exit' to quit.");
							flag=0;
							break;
						}
						else if(choice==2)
							balance-=amount;
						else if(choice==1)
							balance +=amount;

						updateCustomerDatabase(username,choice,balance);
						sendMsgtoClient(client_fd,"User updated successfully.\n--------------------\n\nEnter username of the account holder or 'exit' to quit.");
						flag=0;
						break;
					}

				}
			}

		}
	}
}
/*
	adminRequests() will firslty look wheter the customer is 
	admin or not, if he is admin the it will call adminUtility() to 
	achieve all the fucntionality of the banking admin.
*/
void adminRequests(int client_fd)
{
	sendMsgtoClient(client_fd,"Enter username of the account holder or 'exit' to quit");

	while(1)
	{
		string buffer;
		buffer = recieveMsgFromClient(client_fd);


		if(buffer == "exit")
			break;
		else if(check_if_customer(buffer))
		{
			string userreq;
			userreq = buffer;

			if(adminUtility(userreq,client_fd)==EXIT)
				break;
		}
		else
			sendMsgtoClient(client_fd,"Wrong Username. Please enter a valid user");
	}

}
/*
		getAllCustomerBalace() will help in getting the balance of all the
		customers available in the bank.
*/
string getAllCustomerBalance()
{
	ifstream fin("login_file");
	string line;
   
	string retstr;
	retstr = "";
	
	while(fin) 
	{
		getline(fin,line);
		vector<string> token;
		stringstream tline(line); 
      
    		string tmp; 
      
    		while(getline(tline, tmp , ' ')) 
    		{ 
       			token.push_back(tmp); 
   		} 
		if(token.size() == 0)
			break;
		if(token[2] == "C")
		{	retstr +="UserID: ";
			retstr += token[0];
			retstr =retstr+ " ......"+" bal: ";
			
			retstr += getBalance(token[0]);
			retstr += "\n";
      	        }
    }
    fin.close();
    return retstr;
}	

/*
	policeRequests() will help in achieving all the functionality
	of the police.
*/
void policeRequests(int client_fd)
{
	sendMsgtoClient(client_fd, "Enter your choice\n1. Print Balance of all users\n2. Get mini Statement\n3. To exit the system.");
	int flag=1;

	while(flag)
	{
		string buffer;
		buffer = recieveMsgFromClient(client_fd);
		string balance,ministmt;
		
		balance = "------------------\nAvailable Balance: \n";
		ministmt = "------------------------\nMini Statement of ";
		int choice = stoi(buffer);
		if(choice == 3)
			break;
		else
		{
		
			if(choice == 1)
			{
				balance += getAllCustomerBalance();
				sendMsgtoClient(client_fd,balance+"\n--------------------\n\nEnter your choice\n1. Print Balance of all users\n2. Get mini Statement\n3. To exit the system");
			}
			else if(choice == 2)
			{
				sendMsgtoClient(client_fd,"Enter Username or exit to terminate");

				while(1)
				{
					buffer = recieveMsgFromClient(client_fd);

					if(buffer == "exit")
					{
						flag=1;
						break;
					}
					else if(check_if_customer(buffer))
					{
						string username;
                        ministmt=ministmt+" "+buffer+" :"+'\n'+"---------------------------\n";
						username = buffer;
						if(allCustomers.find(username)!=allCustomers.end())
							ministmt += getMiniStatement(username,client_fd);
						else ministmt="\n>>>Entered customer doesn't exist.\n";
						sendMsgtoClient(client_fd,ministmt + "\n--------------------\n\nEnter your choice\n1. Print Balance of all users\n2. Get mini Statement\n3. To exit the system");
						break;
					}
					else
						sendMsgtoClient(client_fd,"Wrong Username. Please enter a valid user");
				}
			}
		}
	}
}

void closeclient(int client_fd,string str)
{
	sendMsgtoClient(client_fd, str);
    shutdown(client_fd, SHUT_RDWR);
}


int main(int argc,char *argv[]){
    if(argc <2){
        
        cout<<"Port number not provided...\n Program Terminated..."<<endl;
        exit(1);
    }
    cout<<"Ready to accept your query: "<<endl;
    int sockfd,newsockfd,portno,n;
    char buffer[255];
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t clilen;//it is a datatype
    sockfd=socket(AF_INET,SOCK_STREAM,0);//TCP CONNECTION
    if(sockfd<0){
        error("Error opening socket");
    }
    bzero((char *)&serv_addr,sizeof(serv_addr));
    portno=atoi(argv[1]);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(portno);
    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
        error("Binding failed");

    listen(sockfd,5);
    clilen=sizeof(cli_addr);
    
    if(newsockfd<0)
        error("Error on Accept");
    while(1){

        memset(&cli_addr, 0, sizeof(cli_addr));
          newsockfd=accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
          if(newsockfd<0){
              error("Error in accepting.");
          }
           bankClientHandler(newsockfd);

    }
    close(newsockfd);
    close(sockfd);

    return 0;

}
