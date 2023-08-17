#include <fstream>
using namespace std;

#include<string>
#include<algorithm>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <unordered_set>
#include <unordered_map>

#include <dirent.h>
#include <sys/types.h>
#include <typeinfo>


#include<iostream>
#include<cstring>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
#include<errno.h>

//char client_message[2000];
//char *buffer;
string filenames[1000];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
string data[1000];
int file[1000];
int file2[1000];
string myneighports;
unordered_map<string, string> depth2;
unordered_set<string> myneighs;

int x=0;//index of depth 2 neighbours

void *client(void *arg){
   int i = 0;
   int p[stoi(data[3])];
   int clientSocket[stoi(data[3])];
   struct sockaddr_in serverAddr;
   socklen_t addr_size;
   char bufferc[1024];

  // 1. cout << "entered client thread" << endl;
   
   while(i<stoi(data[3]) )
   {
     
     clientSocket[i] = socket(PF_INET, SOCK_STREAM, 0);
     serverAddr.sin_family = AF_INET;

     //Set port number, using htons function 
     serverAddr.sin_port = htons(stoi(data[5+2*i]));


    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;
    while(1){
      if(connect(clientSocket[i], (struct sockaddr *) &serverAddr, addr_size)!=-1){
        break;
      }
    }
  //  cout << "connected to" << data[4+2*i] << endl;
  // if(l<0){cout << strerror(errno);}
  
    char depmess[]="2";
    send(clientSocket[i],depmess,strlen(depmess),0);//sends the depth of my neighbour 

    // cout << "sent depth of it as 1" << endl;

    recv(clientSocket[i], bufferc, 1024, 0) ;//recieves unique id of neighbour
    string a= bufferc;
    
    cout << "Connected to "  <<  data[4+2*i] << " with unique-ID " << a;
    //printf("%s",bufferc);
    cout << " on port " <<  data[5+2*i] << endl;
    
    send(clientSocket[i],data[4+2*stoi(data[3])].c_str(),15,0);//sends total no of files it wants to search

    recv(clientSocket[i],bufferc,1024,0);
    

    string depth2neigh=bufferc;
    string delimiter=" ";
    string token;
    string id;
    size_t pos = 0;
    while ((pos = depth2neigh.find(delimiter)) != std::string::npos) {
      token = depth2neigh.substr(0, pos);
      depth2neigh.erase(0, pos + delimiter.length());
      if(x%2==1){
        if (myneighs.find(token) == myneighs.end()){
          if(depth2.find(token)==depth2.end()){
            depth2[id]=token;
          }
        }
      }
      else{id=token;}
      x++;
    }

    for(int j=0;j<stoi(data[4+2*stoi(data[3])]);j++){
     
	      send(clientSocket[i],data[5+2*stoi(data[3])+j].c_str(),15,0);
	      recv(clientSocket[i], bufferc, 1024, 0);
	      
	      string b=bufferc;
	      // cout << b << endl;
	      if(b=="true"){
	      	if(stoi(a)<file[j]){
		        file[j]=stoi(a);
		      }
	      }
    }
  
  // cout << "Completed neighbour" << data[4+2*i] << endl;
  
   
   close(clientSocket[i]);
   i++;
   }
   for(int i=0;i<stoi(data[4+2*stoi(data[3])]);i++){
    if(file[i]!=10000){
      cout << "Found " << data[5+2*stoi(data[3])+i] << " at " << file[i] << " with MD5 " << "0" << " at depth " << "1" << endl;
    }
   }

   i=0;
      for (auto p:depth2)
      {
        clientSocket[i] = socket(PF_INET, SOCK_STREAM, 0);
        serverAddr.sin_family = AF_INET;
        //Set port number, using htons function 
        serverAddr.sin_port = htons(stoi(p.second));
        //Set IP address to localhost
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

        //Connect the socket to the server using the address
        addr_size = sizeof serverAddr;
        while(1){
          if(connect(clientSocket[i], (struct sockaddr *) &serverAddr, addr_size)!=-1){
            break;
          }
        }
        // cout << "connected to" << data[4+2*i] << endl;;
        // if(l<0){cout << strerror(errno);}
        char depmess[]="2";
        send(clientSocket[i],depmess,strlen(depmess),0);
        recv(clientSocket[i], bufferc, 1024, 0) ;
        string a= bufferc;
    
        send(clientSocket[i],data[4+2*stoi(data[3])].c_str(),15,0);//sends total no of files it wants to search

    for(int j=0;j<stoi(data[4+2*stoi(data[3])]);j++){
     
	      send(clientSocket[i],data[5+2*stoi(data[3])+j].c_str(),15,0);
	      recv(clientSocket[i], bufferc, 1024, 0);
	      
	      string b=bufferc;
	      // cout << b << endl;
	      if(b=="true"){
		      if(stoi(a)<file2[j]){
		        file2[j]=stoi(a);
		      }
	      }
    }
  
  // cout << "Completed neighbour" << data[4+2*i] << endl;
    close(clientSocket[i]);
    i++;
   }

   for(int i=0;i<stoi(data[4+2*stoi(data[3])]);i++){
    if(file[i]==10000&&file2[i]!=10000){
      cout << "Found " << data[5+2*stoi(data[3])+i] << " at " << file2[i] << " with MD5 " << "0" << " at depth " << "1" << endl;
    }
    else if(file[i]==10000&&file2[i]==10000){
       cout << "Found " << data[5+2*stoi(data[3])+i] << " at " << "0" << " with MD5 " << "0" << " at depth " << "0" << endl;
    }
   }

  // sleep(20);
  i = 0;

}



int main(int argc,char *argv[]){

  //collecting data
  ifstream indata;
  indata.open(argv[1]);
  int j=0;
  string word;  
  while(indata >> word){
      data[j]=word;
      j++;
  }
    indata.close();

  for(int j=4;j<4+2*stoi(data[3]);j+=2){
    myneighs.insert(data[j]);
  }
  
  indata.open(argv[1]);
  string line;
  j=0;
  while(getline(indata,line)){
    if(j==2){
      cout << line << endl;
      myneighports=line;
      break;
    }
    j++;
  }
  indata.close();

  for(int j=0;j<stoi(data[4+2*stoi(data[3])]);j++){
      file[j]=10000;
      file2[j]=10000;
  }

  int totfiles=0;
  std::string path = argv[2];
  for (const auto & entry : fs::directory_iterator(path)){
		filenames[totfiles] = entry.path().filename().string();
    totfiles++;
	}

  sort(filenames,filenames+totfiles);
    for(int j=0;j<totfiles;j++){
        cout << filenames[j] << endl;
    }


  int serverSocket;
  struct sockaddr_in Addr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  //Create the socket. 
  serverSocket = socket(PF_INET, SOCK_STREAM, 0);
 
  // Configure settings of the server address struct
  // Address family = Internet 
  Addr.sin_family = AF_INET;

  //Set port number, using htons function to use proper byte order 
  Addr.sin_port = htons(stoi(data[1]));

  //Set IP address to localhost 
  Addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  //Set all bits of the padding field to 0 
  memset(Addr.sin_zero, '\0', sizeof Addr.sin_zero);

 //Bind the address struct to the serversocket 
  bind(serverSocket, (struct sockaddr *) &Addr, sizeof(Addr));

  pthread_t ctid;
  if( pthread_create(&ctid, NULL, client, NULL) != 0 ){
  }

  char client_message[1024];
  char buffer[1024];
  //Listen on the socket, with 100 max connection requests queued 
  listen(serverSocket,100);
  cout << "Listening" ;
  int newSocket[60];
  int i = 0;
  while(1)
    {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket[i] = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        
        
        if(newSocket[i]!=-1){
          char mydepth[5];
          recv(newSocket[i],mydepth,5,0);
	  send(newSocket[i],data[2].c_str(),1024,0);//sends my unique id
	  recv(newSocket[i],client_message,1024,0);//recieves total no of files the client is willing to search

      string mydep=mydepth;
      if(mydep=="1"){
        send(newSocket[i],myneighports.c_str(),strlen(myneighports.c_str()),0);//sends list of my neighbours
      }
  	  string totsfiles=client_message;
  	  for(int j=0;j<stoi(totsfiles);j++){

     		recv(newSocket[i],buffer,1024,0);
     		string filen=buffer;
		string t;
		int k=0;
		while(k<totfiles){
		  if(filenames[k]==filen){
		    t="true";
		    break;
		  }
		  k++;
		}
		if(k==totfiles){
		  t="false";
		}
		send(newSocket[i],t.c_str(),1024,0);//sends present or not present.
	}



        close(newSocket[i]);
        i++;
      }
   }
  return 0;
  }

