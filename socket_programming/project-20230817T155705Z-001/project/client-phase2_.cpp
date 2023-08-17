#include <fstream>
using namespace std;

#include<algorithm>
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
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
string data[1000];
int file[1000];
string filenames[1000];
int totfiles;
string filen;

void * serverThread(void *arg)
{
  int newSocket = *((int *)arg);

  char buffer[1024];
  send(newSocket,data[2].c_str(),1024,0);

  // Send message to the client socket 

  pthread_mutex_lock(&lock);
  recv(newSocket,client_message,1024,0);
  string totsfiles=client_message;
  // cout << totsfiles.length() << endl;
  pthread_mutex_unlock(&lock);

   for(int j=0;j<stoi(totsfiles);j++){

     recv(newSocket,buffer,1024,0);
     filen=buffer;
//   // strcpy(message,data[1]);
//   // strcat(message,"\n");
//   // free(message);
 
  //sleep(1);
 
  
    string t;
    int i=0;
    while(i<totfiles){
      if(filenames[i]==filen){
        t="true";
        break;
      }
      i++;
   }
    if(i==totfiles){
      t="false";
    }

    send(newSocket,t.c_str(),1024,0);
   }

  
  
  // cout << "Exit serverThread \n";
  close(newSocket);
  pthread_exit(NULL);
}

void *clientThread(void *arg){
  // cout << "In thread\n";
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

 //Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;
    while(1){
      if(connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size)!=-1){break;}
    }
    int l=recv(clientSocket, bufferc, 1024, 0) ;

    cout << "Connected to "  <<  data[4+2*i] << " with unique-ID " ;
     string a=bufferc;
     cout << a;
    cout << " on port " <<  data[5+2*i] << endl;

    send(clientSocket,data[4+2*stoi(data[3])].c_str(),15,0);

    for(int j=0;j<stoi(data[4+2*stoi(data[3])]);j++){
     
      send(clientSocket,data[5+2*stoi(data[3])+j].c_str(),15,0);
      recv(clientSocket, bufferc, 1024, 0);
      
      string b=bufferc;
      // cout << b << endl;
      if(b=="true"){
        pthread_mutex_lock(&lock);
        if(stoi(a)<file[j]){
          file[j]=stoi(a);
        }
        pthread_mutex_unlock(&lock);
      }
    }

    // cout << "Completed neighbour" << data[4+2*i] << endl;
    close(clientSocket);
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
  // sleep(20);
  i = 0;

  // struct timespec ts;
  // ts.tv_sec=60;
  // cout << "threads done" << endl;
  for(int i=0;i<stoi(data[4+2*stoi(data[3])]);i++){
    if(file[i]==10000){
      cout << "Found " << data[5+2*stoi(data[3])+i] << " at " << "0" << " with MD5 " << "0" << " at depth " << "0" << endl;
    }
    else{
      cout << "Found " << data[5+2*stoi(data[3])+i] << " at " << file[i] << " with MD5 " << "0" << " at depth " << "1" << endl;
    }
  }
}



int main(int argc,char *argv[]){

  //collecting data
  ifstream indata;
  indata.open(argv[1]);
  int j=0;
  string word;  
  while(indata >> word){
      // cout << word.length() << endl;
      data[j]=word;
      j++;
  }
  indata.close();

  for(int j=0;j<stoi(data[4+2*stoi(data[3])]);j++){
      file[j]=10000;
  }

  totfiles=0;
  std::string path = argv[2];
  for (const auto & entry : fs::directory_iterator(path)){
		filenames[totfiles] = entry.path().filename().string();
		// std::cout << filenames[totfiles] << std::endl;
    totfiles++;
	}

  sort(filenames,filenames+totfiles);
    for(int j=0;j<totfiles;j++){
        cout << filenames[j] << endl;
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

  //Listen on the socket, with 100 max connection requests queued 
  if(listen(serverSocket,100)==0){
    // printf("Listening\n");
    }
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
        // cout << i << endl;
        
    }
  return 0;
}

