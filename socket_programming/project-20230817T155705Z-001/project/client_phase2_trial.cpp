#include <fstream>
using namespace std;

#include<string>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

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

char client_message[2000];
char *buffer;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
string data[1000];
bool filepresent[1000];
string dirfiles[1000];
int numdirfiles;

void * serverThread(void *arg)
{
  int newSocket = *((int *)arg);

  // Send message to the client socket 
  //pthread_mutex_lock(&lock);

//
   cout << data[2];

  // strcpy(message,data[1]);
  // strcat(message,"\n");
  // free(message);
 // pthread_mutex_unlock(&lock);
  sleep(1);
  send(newSocket,data[2].c_str(),13,0);
  for(int i=0;i<20;i++){
  		cout<< "server loop"<<i<<endl;
	  char sbuf[1024];
	  if(recv(newSocket,sbuf,1023,0)==-1){
	  	cout<<"server recieve error"<<strerror(errno)<<endl;}
	  else{
		  string sispresent;
		  string filereq=sbuf;
		  int found = 0;
		  for(int i=0;i<numdirfiles;i++){
		  	if(filereq==dirfiles[i]){
		  		found=1;
		  		
		  		if(send(newSocket,"true",4,0)==-1){
		  			cout<<"server send error"<<strerror(errno)<<endl;}
		  		break;
		  	}
		  }
		  if(found==0){
		  	if(send(newSocket,"false",5,0)==-1){
		  		cout<<"server send error"<<strerror(errno)<<endl;}
			}
	}
 }
  
  close(newSocket);
  cout << "Exit serverThread \n";
  pthread_exit(NULL);
}

void *clientThread(void *arg){
  cout << "In thread\n";
  char messagec[1000];
  char bufferc[1024];
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  int i = *((int *)arg);

  // Create the socket. 
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  //Configure settings of the server address
 // Address family is Internet 
  serverAddr.sin_family = AF_INET;

  //Set port number, using htons function 
  serverAddr.sin_port = htons(stoi(data[5+2*i]));

  cout<<i<< " " << data[5+2*i] <<endl;

 //Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;
    while(1){
      if(connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size)!=-1){break;}
      //else{cout << strerror(errno);}
    }
    int l=recv(clientSocket, bufferc, 1024, 0) ;
     if(l < 0)
    {
       cout << "Receive failed\n";
    }

    int numclients=stoi(data[3]);
    int numfiles=stoi(data[4+2*numclients]);
    for (int i=0;i<numfiles;i++){
    	const char * msg = data[5+2*numclients+i].c_str();
    	int len = strlen(msg);
    	char buf[1024];
    	if(send(clientSocket,msg,len,0)==-1){
    		cout<<"client send error"<<strerror(errno)<<endl;}
    	if(recv(clientSocket,buf,1023,0)==-1){
    		
    		cout<<"client recieve error"<<strerror(errno)<<endl;}
    	string ispresent = buf;
    	cout<<"from server"<< ispresent<<endl;
    	if (ispresent == "true" ){
    		filepresent[i]=1;
    		cout << "found";
    	}
    }
    

    cout << "Connected to "  <<  data[4+2*i] << " with unique-ID " ;
     printf("%s",bufferc);
    cout << " on port " <<  data[5+2*i] << endl;
    close(clientSocket);
    cout<<"exit client thread"<<endl;
    pthread_exit(NULL);


}

void *client(void *arg){
   int i = 0;
   int p[stoi(data[3])];
   pthread_t tid[stoi(data[3])];
   while(i<stoi(data[3]) )
   {
     p[i]=i;
     if( pthread_create(&tid[i], NULL, clientThread,&p[i] ) != 0 )
           {printf("Failed to create thread\n");}
     i++;
   }
  sleep(20);
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

  std::string path = argv[2];
  numdirfiles=0;
  for (const auto & entry : fs::directory_iterator(path)){
    		
		dirfiles[numdirfiles] = entry.path().filename().string();
		std::cout << dirfiles[numdirfiles] << std::endl;
		numdirfiles++;
	}


  int serverSocket, newSocket;
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
    printf("Failed to create client\n");
  }

  //Bind the address struct to the cliensocket 
 // bind(clientSocket, (struct sockaddr *) &Addr, sizeof(Addr));

  //Listen on the socket, with 40 max connection requests queued 
  if(listen(serverSocket,50)==0){
    printf("Listening\n");}
  else{
    printf("Error\n");}
    pthread_t tid[60];
    int i = 0;
    while(1)
    {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        //for each client request creates a thread and assign the client request to it to process
       //so the main thread can entertain next request
        if(newSocket!=-1){
        if( pthread_create(&tid[i++], NULL, serverThread, &newSocket) != 0 )
           {printf("Failed to create thread\n");}
        }
        else{cout << strerror(errno);}
        cout << "main accept loop"<<i << endl;
        
    }
 
  return 0;
}

