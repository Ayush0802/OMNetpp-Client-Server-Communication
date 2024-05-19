#include "client.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <functional>
#include <chrono>
#include <cmath>
#include <fstream>
#include <random>
using namespace std;

Define_Module(Client);

void Client::initialize()
{
    string filePath = "client_outputs/client" + to_string(getIndex()) + ".txt";
    clientout.open(filePath);

    clientId = par("client_id");
    n_clients = par("x");
    count = 0;
    result =0;
    n_subarray = 2*n_clients;
    array_size = 3*n_subarray;

    subarray_max.assign(n_subarray,0);

    cout<<endl; clientout<<endl;
    cout<<"for client : "<<clientId<<endl;
    clientout<<"for client : "<<clientId<<endl;
    for(int i=0;i<array_size;i++){                 // initialize a random array for client
        globalArray[i] = rand() % 100;
        cout<<globalArray[i]<<" ";
        clientout<<globalArray[i]<<" ";
    }
    cout<<endl; clientout<<endl;

    m = n_clients;
    p = ceil(log2(m));
    conn_mat.assign(p, {0, 0});

    for(int i=1;i<=p;i++){                          // conn_mat stores the client ids to which the client is connected
        int t = (clientId + static_cast<int>(pow(2, (i-1)))) % m;
        conn_mat[i-1].first = t;
        conn_mat[i-1].second = i-1;
    }


    sort(conn_mat.begin(), conn_mat.end());         // sorting the conn_mat

    cout<<"connections : ";
    clientout<<"connections : ";
    for(int i=0;i<p;i++){
        cout<<"("<<conn_mat[i].first<<" "<<conn_mat[i].second<<") ";
        clientout<<"("<<conn_mat[i].first<<" "<<conn_mat[i].second<<") ";
    }
    cout<<endl; clientout<<endl;

    send_array_to_client();

}

void Client::send_array_to_client()
{
    int startIndex = 0;
    int subarraySize = array_size / n_subarray;

    for (int i = 0; i < n_subarray; i++) {                  // Create subarray and send to other client

      int* subarray = new int[subarraySize+4];
      std::copy(globalArray + startIndex, globalArray + startIndex + subarraySize, subarray);
      startIndex += subarraySize;

      int sub_id;
      int actual_client_to_send;
      while(1)
      {
          sub_id = rand()%100;
          if(sub_id%n_clients!=clientId)
          {
              break;
          }
      }
      subarray[subarraySize] = sub_id;                      // append the subarray id
      actual_client_to_send = sub_id%n_clients;             // destination client id

      subarray[subarraySize+1] = actual_client_to_send;     // append the destination client id
      subarray[subarraySize+2] = clientId;                  // append the source client id
      subarray[subarraySize+3] = -1;                        // set the mode to -1

      int idx = calculate_index(actual_client_to_send);     // calculate the next client id in path of sorce to destination


      send( createArrayMessage(subarray, subarraySize+4), "client_out", idx);       // send the subarray

      cout<<"subarray send: ";
      clientout<<"subarray send: ";
      for(int k=0;k<subarraySize+4;k++){
          cout<<subarray[k]<<" ";
          clientout<<subarray[k]<<" ";
      }
      cout<<endl; clientout<<endl;

    }

}

void Client::handleMessage(cMessage *msg)
{
    int* receivedArray = static_cast<int*>(msg->getContextPointer());
    int arraySize = msg->getKind();

    int mode = *(receivedArray+arraySize-1);            // Extract mode from received array

    if(mode == -1)                                  // mode = -1 : subarray is traveling
    {
        int client_to_reach = *(receivedArray+arraySize-3);

        if(client_to_reach != clientId){                        // the client is intermediate
                cout<<"subarr rcv and forwarded"<<endl;
                clientout<<"subarr rcv and forwarded"<<endl;

                int idx = calculate_index(client_to_reach);

                send( createArrayMessage(receivedArray, arraySize), "client_out", idx);

        }
        else{                                           // the client destination got reached, now calulate the max
                int maxElement = -1;
                for(int i=0;i<arraySize-4;i++){
                   int x = *(receivedArray+i);
                   if(maxElement < x){
                       maxElement = x;
                   }
                }

                int subarrayId = *(receivedArray+arraySize-4);      // extract subarray id
                int source = *(receivedArray+arraySize-2);          // extract source client id

                int* maxArray = new int[5];          // Create an array to store the max value and other information
                maxArray[0] = maxElement;            // store max value
                maxArray[1] = subarrayId;            // store subarray id
                maxArray[2] = source;                // store the previous source as new destination
                maxArray[3] = clientId;              // store previous destination as source
                maxArray[4] = -2;                    // set mode -2

                int idx = calculate_index(source);

                send( createArrayMessage(maxArray, 5), "client_out", idx);

                cout<<"max arr send: ";
                clientout<<"max arr send: ";
                for(int i=0;i<5;i++){
                    cout<<maxArray[i]<<" ";
                    clientout<<maxArray[i]<<" ";
                }
                cout<<endl; clientout<<endl;

        }
    }
    if(mode == -2){                                                 // mode -1 means the max of subarray is travelling
        int client_to_reach = *(receivedArray+arraySize-3);

        if(client_to_reach != clientId)                             // the client is intermediate
        {
                cout<<"max arr rcv and forwarded"<<endl;
                clientout<<"max arr rcv and forwarded"<<endl;
                int idx = calculate_index(client_to_reach);

                send( createArrayMessage(receivedArray, arraySize), "client_out", idx);
        }
        else                                                        // got the client from which subarray was generated
        {
                int subarrayId = *(receivedArray+arraySize-4);
                int source = *(receivedArray+arraySize-3);
                int ans = *(receivedArray+arraySize-5);             // extracte the max value

                cout<<"result arr rcv : ";
                clientout<<"result arr rcv : ";
                for(int i=5;i>=1;i--){
                    cout<<*(receivedArray+arraySize-i)<<" ";
                    clientout<<*(receivedArray+arraySize-i)<<" ";
                }
                cout<<endl; clientout<<endl;

                count++;
                subarray_max.push_back(ans);                        // push max value to the subarray max vector

                if(count == n_subarray){                        // all subarray max recieved
                    result = *max_element(subarray_max.begin(),subarray_max.end());

                    cout<<"all subarray rcvd by "<<clientId<< " , final ans : "<<result<<endl<<endl;
                    clientout<<"all subarray rcvd by "<<clientId<< " , final ans : "<<result<<endl<<endl;

                    sendGossip();

                }
        }
    }
    if(mode==-3)                                // handle gossip
    {
        std::stringstream ss;
        ss << "<";
        ss << "'" << *(receivedArray+arraySize-4) <<"'" << ":";
        ss << *(receivedArray+arraySize-3) << ":";
        ss << *(receivedArray+arraySize-2);
        ss << ">";

        size_t hash = std::hash<std::string>{}(ss.str());

        if (find(hashList.begin(), hashList.end(), hash) == hashList.end()) {           // condition if gsp not present in hash

            cout<<"gsp rcv : "<<endl;
            clientout<<"gsp rcv : ";
            for(int i=4;i>=1;i--){
                cout<<*(receivedArray+arraySize-i)<<" ";
                clientout<<*(receivedArray+arraySize-i)<<" ";
            }
            cout<<endl; clientout<<endl;


            int varb = *(receivedArray+arraySize-3);
            hashList.push_back(hash);
            for(int i=0;i<p;i++)
            {
                if(i!=varb)
                {
                    send( createArrayMessage(receivedArray,4), "client_out",i);
                }
            }
        }

    }

}
void Client::sendGossip()
{
    auto now = std::chrono::system_clock::now();
    time_t timestamp = std::chrono::system_clock::to_time_t(now);
    int timestampInt = static_cast<int>(timestamp);

//    cout<<endl<<clientId<<" "<<timestampInt<<endl;
    int* gossip_array = new int[4];
    gossip_array[0] = timestampInt+clientId;
    gossip_array[1] = clientId;
    gossip_array[2] = result;
    gossip_array[3] = -3;

    std::stringstream ss;
    ss << "<";
    ss << "'" << timestampInt+clientId <<"'" << ":";
    ss << clientId << ":";
    ss << result;
    ss << ">";

    size_t hash = std::hash<std::string>{}(ss.str());
    hashList.push_back(hash);

    for(int i=0;i<p;i++)
    {
        send( createArrayMessage(gossip_array,4), "client_out",i);
    }
}

int Client::calculate_index(int destination){
    int next_client = -1;
    int idx = 0;
    for(int i=0;i<p;i++){
      if(destination >= conn_mat[i].first){
          next_client = conn_mat[i].first;
          idx = conn_mat[i].second;
      }
    }
    if(next_client == -1)
    {
      next_client = conn_mat[p-1].first;
      idx = conn_mat[p-1].second;
    }
    return idx;
}

cMessage* Client::createArrayMessage(int* array, int size)
{
   cMessage* msg = new cMessage("ArrayMessage");
   msg->setContextPointer(array);
   msg->setKind(size);
   return msg;
}

Client::~Client()
{
   delete[] originalArray;
}
