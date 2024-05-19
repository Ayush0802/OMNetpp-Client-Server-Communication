#include "server.h"
#include <algorithm>
#include <cstdlib> // For random number generation
#include <ctime>   // For seeding random number generator
#include <fstream>

using namespace std;

Define_Module(Server);

void Server::initialize()
{
    serverId = par("server_id");
    n_server = par("n");
    numserver = n_server / 2 + 1;
    numClients = par("x");
    string filePath = "server_outputs/server" + to_string(getIndex()) + ".txt";
    serverout.open(filePath);

}

void Server::handleMessage(cMessage *msg)
{
    int* receivedArray = static_cast<int*>(msg->getContextPointer());
    int arraySize = msg->getKind();

    if (receivedArray == nullptr) {             // Handle null pointer gracefully
        std::cerr << "Received null array\n";
        return;
    }

    int subarrayId = *(receivedArray+arraySize-1); // Extract subarray ID from the last element
    bool malicious = (serverId % 4 == subarrayId % 4);


    int maxElement = -1;
    int minElement = 9999;
    cout<< "server rcv ";
    serverout<< "server rcv ";
    for(int i=0;i<arraySize-1;i++){
       int x = *(receivedArray+i);
       cout<<x<<" ";
       serverout<<x<<" ";
       if(maxElement < x){
           maxElement = x;
       }
       if(minElement > x){
          minElement = x;
      }
    }
    cout<<subarrayId<<endl;
    serverout<<subarrayId<<endl;

    int ans = -1;
    if(malicious==true){
        ans = minElement;
    }
    else{
        ans = maxElement;
    }

    int* resultArray = new int[2];          // Create an array to store the result
    resultArray[0] = ans;
    resultArray[1] = subarrayId;

    cout<< "server send " <<", is malicious : "<<malicious<<", subarray : "<< ans<< " " <<subarrayId <<endl;
    serverout<< "server send " <<", is malicious : "<<malicious<<", subarray : "<< ans<< " " <<subarrayId <<endl;
    send(createMessage(resultArray, 2), "server_out", msg->getArrivalGate()->getIndex());

}


cMessage* Server::createMessage(int* array, int size)
{
    cMessage* msg = new cMessage("ArrayMessage");
    msg->setContextPointer(array);
    msg->setKind(size);
    return msg;
}
